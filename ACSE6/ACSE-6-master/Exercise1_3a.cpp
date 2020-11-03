#include <mpi.h>
#include <iostream>
#include <cstdlib>

using namespace std;

int id, p;
int tag_num = 1;

int main(int argc, char *argv[])
{
	int recv_cnt = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	for (int i = 0; i < p; i++)
	{
		if (i == id)
		{
			for (int j = 0; j < p; j++)
				if (j != id)
				{
					double send_data = (double)id / (double)p;
					MPI_Send(&send_data, 1, MPI_DOUBLE, j, tag_num, MPI_COMM_WORLD);
				}
		}
		else
		{
			double recv_data;
			MPI_Recv(&recv_data, 1, MPI_DOUBLE, i, tag_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			cout << id << " received " << recv_data << " from " << i << endl;
		}

		tag_num++;
	}

	MPI_Finalize();
}