#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>
using namespace std;

int id, p;
int tag_num = 1;

int main(int argc, char *argv[])
{
	int recv_cnt = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL)+id*100);

	int source = (id + p - 1) % p;
	int destination = (id + 1) % p;
	int data_size = id+1;
	if (id == 0) data_size = p;

	int *data = new int[data_size];

	if (id == 0)
	{
		data[0] = rand();
		MPI_Send(data, 1, MPI_INT, destination, tag_num, MPI_COMM_WORLD);
		MPI_Recv(data, p, MPI_INT, source, tag_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		cout << "Process id received:";
		for (int i = 0; i < p; i++)
			cout << "\t" << data[i];
		cout << endl;
	}
	else
	{
		MPI_Recv(data, data_size-1, MPI_INT, source, tag_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		data[data_size - 1] = rand();
		MPI_Send(data, data_size, MPI_INT, destination, tag_num, MPI_COMM_WORLD);
		cout << "Process " << id << " added " << data[data_size - 1] << endl;
	}

	MPI_Finalize();
}