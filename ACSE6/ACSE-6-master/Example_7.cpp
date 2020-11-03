#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

int id, p;

void Do_Work(void)
{
	int sum = 0;
	for (int i = 0; i < 100; i++) sum = sum + 10;		//Some (not very useful) work
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	int tag_num = 1, sent_num = 100000, cnt = 0, flag = 0;

	if (id == 0)
	{
		MPI_Request* request = new MPI_Request[p - 1];
		int **send_data = new int*[p - 1];
		for (int i = 0; i < p - 1; i++)
		{
			send_data[i] = new int[sent_num];
			for (int j = 0; j < sent_num; j++)
				send_data[i][j] = rand();
		}

		for (int i = 1; i < p; i++)
			MPI_Isend(send_data[i-1], sent_num, MPI_INT, i, tag_num, MPI_COMM_WORLD, &request[i - 1]);

		while (MPI_Testall(p - 1, request, &flag, MPI_STATUS_IGNORE) == MPI_SUCCESS && flag == 0)
		{
			Do_Work();
			cnt++;
		}

		for (int i = 0; i < p - 1; i++)
			delete[] send_data[i] ;
		delete[] send_data;
		delete[] request;
	}
	else
	{
		int *recv_data=new int[sent_num];
		MPI_Request request;
		MPI_Irecv(recv_data, sent_num, MPI_INT, 0, tag_num, MPI_COMM_WORLD, &request);

		while (MPI_Test(&request, &flag, MPI_STATUS_IGNORE) == MPI_SUCCESS && flag == 0)
		{
			Do_Work();
			cnt++;
		}

		delete[] recv_data;
	}

	cout << "Process " << id << " did " << cnt << " cycles of work while wating for the communication to finish" << endl;
	cout.flush();

	MPI_Finalize();
}