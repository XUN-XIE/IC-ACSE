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
	srand(time(NULL)+id*10);

	int tag_num = 1;

	if (id == 0)
	{
		for (int i = 1; i < p; i++)
		{
			int send_data = rand();
			MPI_Send(&send_data, 1, MPI_INT, i, tag_num, MPI_COMM_WORLD);
			cout << send_data << " sent to processor " << i << endl;
		}
	}
	else
	{
		int recv_data;
		MPI_Recv(&recv_data, 1, MPI_INT, 0, tag_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		cout << recv_data << " received on processor " << id << endl;
	}

	MPI_Finalize();
}