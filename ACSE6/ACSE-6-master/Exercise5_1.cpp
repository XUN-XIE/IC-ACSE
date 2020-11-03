#include <mpi.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <vector>

//Note that this is the solution to 5_1b (a is a slightly simpler version of this one)

using namespace std;

int id, p, tag_num = 1;

double Random(void)
{
	return (double)rand() / RAND_MAX;
}

//Doing this like this is more complex and thorough than is strictly required by the question
//I have, though, created a more generally useful class to show how it might be done
class CMatrix
{
public:
	//Writing functions that set up a single communication to send/receive either a portion of or all of this matrix object
	void send_matrix_all(int destination, int tag_num, MPI_Request *request)
	{
		send_matrix_rows(destination, tag_num, 0, rows - 1, request);
	}
	void send_matrix_rows(int destination, int tag_num, int min_row, int max_row, MPI_Request *request);
	void send_matrix_columns(int destination, int tag_num, int min_column, int max_column, MPI_Request *request);
	void receive_matrix_all(int source, int tag_num, MPI_Request *request)
	{
		receive_matrix_rows(source, tag_num, 0, rows - 1, request);
	}
	void receive_matrix_rows(int source, int tag_num, int min_row, int max_row, MPI_Request *request);
	void receive_matrix_columns(int destination, int tag_num, int min_column, int max_column, MPI_Request *request);
	void broadcast_matrix(int root, MPI_Request *request);
	vector<double> &operator[](int n)
	{
		return m_data[n];
	}
	int Get_Rows(void)
	{
		return rows;
	}
	int Get_Columns(void)
	{
		return columns;
	}
	void Set_Size(int tot_rows, int tot_columns)
	{
		rows = tot_rows;
		columns = tot_columns;
		m_data.resize(rows, vector<double>(columns));
	}
	//Fill matrix with random numbers
	void Fill_Random(double min, double max);

	//required if swapping type of send use
	void Clear_MPI_Type()
	{
		if (MPI_matrix_type != nullptr)
			MPI_Type_free(MPI_matrix_type);
		delete MPI_matrix_type;
		MPI_matrix_type = nullptr;
	}

	CMatrix()
	{
		MPI_matrix_type = nullptr;
	}
	~CMatrix()
	{
		int flag;
		MPI_Finalized(&flag);
		if (!flag && MPI_matrix_type != nullptr)
			MPI_Type_free(MPI_matrix_type);
		delete MPI_matrix_type;
	}

private:
	//I am using a vector to setup the matrix. It could also easily be done using dynamic allocation
	int rows, columns;
	vector< vector<double> > m_data;

	MPI_Datatype *MPI_matrix_type;
	void setup_MPI_type_rows(int min_row, int max_row);
	void setup_MPI_type_columns(int min_column, int max_column);
};

void CMatrix::setup_MPI_type_rows(int min_row, int max_row)
{
	if (MPI_matrix_type == nullptr)
	{
		vector<int> block_length(max_row - min_row + 1);
		vector<MPI_Aint> addresses(max_row - min_row + 1);
		vector<MPI_Datatype> typelist(max_row - min_row + 1);

		int cnt = 0;
		for (int i = min_row; i <= max_row; i++)
		{
			block_length[cnt] = columns;
			MPI_Aint temp;
			MPI_Get_address(m_data[i].data(), &temp);
			addresses[cnt] = temp;
			typelist[cnt] = MPI_DOUBLE;
			cnt++;
		}

		MPI_matrix_type = new MPI_Datatype;
		MPI_Type_create_struct(cnt, &block_length[0], &addresses[0], &typelist[0], MPI_matrix_type);
		MPI_Type_commit(MPI_matrix_type);
	}
}

void CMatrix::setup_MPI_type_columns(int min_column, int max_column)
{
	if (MPI_matrix_type == nullptr)
	{
		vector<int> block_length(rows);
		vector<MPI_Aint> addresses(rows);
		vector<MPI_Datatype> typelist(rows);

		int cnt = 0;
		for (int i = 0; i < rows; i++)
		{
			block_length[cnt] = max_column - min_column + 1;
			MPI_Aint temp;
			MPI_Get_address(&m_data[i][min_column], &temp);
			addresses[cnt] = temp;
			typelist[cnt] = MPI_DOUBLE;
			cnt++;
		}

		MPI_matrix_type = new MPI_Datatype;
		MPI_Type_create_struct(cnt, &block_length[0], &addresses[0], &typelist[0], MPI_matrix_type);
		MPI_Type_commit(MPI_matrix_type);
	}
}

void CMatrix::send_matrix_rows(int destination, int tag_num, int min_row, int max_row, MPI_Request *request)
{
	setup_MPI_type_rows(min_row, max_row);
	MPI_Send(MPI_BOTTOM, 1, *MPI_matrix_type, destination, tag_num, MPI_COMM_WORLD);
}

void CMatrix::receive_matrix_rows(int source, int tag_num, int min_row, int max_row, MPI_Request *request)
{
	setup_MPI_type_rows(min_row, max_row);
	MPI_Irecv(MPI_BOTTOM, 1, *MPI_matrix_type, source, tag_num, MPI_COMM_WORLD, request);
}

void CMatrix::send_matrix_columns(int destination, int tag_num, int min_column, int max_column, MPI_Request *request)
{
	setup_MPI_type_columns(min_column, max_column);
	MPI_Isend(MPI_BOTTOM, 1, *MPI_matrix_type, destination, tag_num, MPI_COMM_WORLD, request);
}

void CMatrix::receive_matrix_columns(int source, int tag_num, int min_column, int max_column, MPI_Request *request)
{
	setup_MPI_type_columns(min_column, max_column);
	MPI_Irecv(MPI_BOTTOM, 1, *MPI_matrix_type, source, tag_num, MPI_COMM_WORLD, request);
}

void CMatrix::broadcast_matrix(int root, MPI_Request *request)
{
	setup_MPI_type_rows(0, rows-1);
	MPI_Ibcast(MPI_BOTTOM, 1, *MPI_matrix_type, root, MPI_COMM_WORLD, request);
}

void CMatrix::Fill_Random(double min, double max)
{
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < columns; j++)
			m_data[i][j] = min + Random()*(max - min);
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 1000);

	//set dimensions of matrices (C = A*B) ii = rows in A, kk = columns in A and rows in B, jj is columns in B 
	int ii = 200, jj = 100, kk = 150;
	CMatrix A, B, C;

	A.Set_Size(ii, kk);
	B.Set_Size(kk, jj);
	C.Set_Size(ii, jj);

	if (id == 0)
	{
		A.Fill_Random(0.0, 1000.0);
		B.Fill_Random(0.0, 1000.0);
	}

	//This section distributes the regions the processors are responsible for
	//rows in the answer matrix (i.e. range of ii)
	int rows_responsible_min_max[2];
	vector< vector<int> > rows_responsible_list(p, vector<int>(2));

	if (id == 0)
	{
		int rows_left = ii;
		int min_row = 0;
		MPI_Request *requests = new MPI_Request[p - 1];

		for (int i = 0; i < p; i++)
		{
			//This is done in case the number of rows are not exactly divisible by p
			//Better than having all the shortfall accumulate on one processor
			int portion = rows_left / (p - i);
			rows_responsible_list[i][0] = min_row;
			rows_responsible_list[i][1] = min_row + portion - 1;
			min_row += portion;
			rows_left -= portion;
			if (i == id)
			{
				for (int j = 0; j < 2; j++) rows_responsible_min_max[j] = rows_responsible_list[id][j];
			}
			else
			{
				MPI_Isend(&rows_responsible_list[i][0], 2, MPI_INT, i, 3, MPI_COMM_WORLD, &requests[i - 1]);
			}
		}

		MPI_Waitall(p - 1, requests, MPI_STATUSES_IGNORE);

		delete[] requests;
	}
	else
	{
		//Non-blocking doesn't help here
		MPI_Recv(rows_responsible_min_max, 2, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	cout << id << ": " << rows_responsible_min_max[0] << " " << rows_responsible_min_max[1] << endl;

	//Send and receive the appropriate portions of the arrays
	MPI_Request *requests_back = nullptr;
	MPI_Request *requests_out = nullptr;
	if (id == 0)
	{
		requests_out = new MPI_Request[p];
		//We will set up the return requests now to have them ready and waiting for the answers
		requests_back = new MPI_Request[p - 1];
		for (int i = 1; i < p; i++)
		{
			A.send_matrix_rows(i, 1, rows_responsible_list[i][0], rows_responsible_list[i][1], &requests_out[i - 1]);
			C.receive_matrix_rows(i, 2, rows_responsible_list[i][0], rows_responsible_list[i][1], &requests_back[i - 1]);
			A.Clear_MPI_Type();
			C.Clear_MPI_Type();
		}
		B.broadcast_matrix(0, &requests_out[p - 1]);
		MPI_Waitall(p, requests_out, MPI_STATUSES_IGNORE)==MPI_SUCCESS;
		
	}
	else
	{
		requests_out = new MPI_Request[2];
		A.receive_matrix_rows(0, 1, rows_responsible_min_max[0], rows_responsible_min_max[1], &requests_out[0]);
		B.broadcast_matrix(0, &requests_out[1]);
		MPI_Waitall(2, requests_out, MPI_STATUSES_IGNORE);
		
	}

	delete[] requests_out;

	//Do the matrix multiplication on the appropriate portion of the matrix

	for (int i = rows_responsible_min_max[0]; i <= rows_responsible_min_max[1]; i++)
		for (int j = 0; j < jj; j++)
		{
			C[i][j] = 0.0;
			for (int k = 0; k < kk; k++)
				C[i][j] += A[i][k] * B[k][j];
		}
	
	//Send back C (note receives are already set up on 0)
	if (id == 0)
		MPI_Waitall(p - 1, requests_back, MPI_STATUSES_IGNORE);
	else
	{
		requests_back = new MPI_Request[1];
		C.send_matrix_rows(0, 2, rows_responsible_min_max[0], rows_responsible_min_max[1], &requests_back[0]);
		MPI_Waitall(1, requests_back, MPI_STATUSES_IGNORE);
	}

	if (id == 0)
	{
		cout << "Done Parallel, starting serial calculation" << endl;
		cout.flush();
	}

	delete[] requests_back;

	//Going to do the matrix multiplication in serial on processor 0 and report the difference with the parallel calculation as an error check
	
	if (id == 0)
	{
		CMatrix C_serial;
		C_serial.Set_Size(ii, jj);
		double difference = 0.0;

		for (int i = 0; i < ii; i++)
		{
			for (int j = 0; j < jj; j++)
			{
				C_serial[i][j] = 0.0;
				for (int k = 0; k < kk; k++)
				{
					C_serial[i][j] += A[i][k] * B[k][j];
					
				}
				difference += fabs(C_serial[i][j] - C[i][j]);
			}
		}

		cout << "Difference between parallel and serial calculation is " << difference << endl;
	}

	MPI_Finalize();
}