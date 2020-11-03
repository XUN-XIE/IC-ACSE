#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

int id, p;

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	int tag_num = 1;

	if (id == 0)
	{
		MPI_Request* request=new MPI_Request[p-1];
		int *send_data = new int[p-1];

		for (int i = 1; i < p; i++)
		{
			send_data[i - 1] = rand();
			MPI_Isend(&send_data[i-1], 1, MPI_INT, i, tag_num, MPI_COMM_WORLD, &request[i-1]);
			cout << send_data[i-1] << " sent to processor " << i << endl;
		}

		MPI_Waitall(p - 1, request, MPI_STATUS_IGNORE);

		delete[] send_data;
		delete[] request;
	}
	else
	{
		int recv_data;
		MPI_Request request;
		MPI_Irecv(&recv_data, 1, MPI_INT, 0, tag_num, MPI_COMM_WORLD, &request);

		MPI_Wait(&request, MPI_STATUS_IGNORE);

		cout << recv_data << " received on processor " << id << endl;
	}

	MPI_Finalize();
}