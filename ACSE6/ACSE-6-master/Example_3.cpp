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
		for (int i = 1; i < p; i++)
		{
			int num_send = 1 + rand() % 5;		//random number between 1 and 5 - inclusive
			int *send_data = new int[num_send];	//allocate the required memory
			for (int j = 0; j < num_send; j++) send_data[j] = rand();
				
			MPI_Send(send_data, num_send, MPI_INT, i, tag_num, MPI_COMM_WORLD);

			for (int j = 0; j < num_send; j++) cout << send_data[j] << "\t";
			cout << " sent to processor " << i << endl;
			cout.flush();

			delete[] send_data;					//free the allocated memory
		}
	}
	else
	{
		int *recv_data;
		int num_recv;
		MPI_Status status;
		MPI_Probe(0, tag_num, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &num_recv);
		recv_data = new int[num_recv];
		MPI_Recv(recv_data, num_recv, MPI_INT, 0, tag_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (int j = 0; j < num_recv; j++) cout << recv_data[j] << "\t";
		cout << " received on processor " << id << endl;
		cout.flush();
	}

	MPI_Finalize();
}