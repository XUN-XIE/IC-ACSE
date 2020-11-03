#include <mpi.h>
#include <iostream>
#include <cstdlib>

using namespace std;

int id, p;
int tag_num = 1;


void comm_range(int bottom, int top)
{
	if (top == bottom || id<bottom || id>top)
		return;

	int range = (top - bottom) / 2 + 1;
	int mid = bottom + range;		

	if (id < mid)
	{
		int offset = id - bottom;

		for (int i = 0; i < range; i++)
		{
			int other_proc = (offset + i) % range + mid;

			if (other_proc <= top)
			{
				int send_data = id;
				MPI_Send(&send_data, 1, MPI_INT, other_proc, tag_num, MPI_COMM_WORLD);
				cout << "Process " << id << " sent " << send_data << " to process " << other_proc << endl;
				int recv_data;
				MPI_Recv(&recv_data, 1, MPI_INT, other_proc, tag_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				cout << "Process " << id << " received " << recv_data << " from process " << other_proc << endl;
				cout.flush();
			}
		}
	}
	else
	{
		int offset = id - mid;

		for (int i = 0; i < range; i++)
		{
			int other_proc = (offset - i + range) % range + bottom;
			int recv_data;
			MPI_Recv(&recv_data, 1, MPI_INT, other_proc, tag_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			cout << "Process " << id << " received " << recv_data << " from process " << other_proc << endl;
			int send_data = id;
			MPI_Send(&send_data, 1, MPI_INT, other_proc, tag_num, MPI_COMM_WORLD);
			cout << "Process " << id << " sent " << send_data << " to process " << other_proc << endl;
			cout.flush();
		}
	}

	comm_range(bottom, mid - 1);
	comm_range(mid, top);
}



int main(int argc, char *argv[])
{
	int first_comm, second_comm;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	comm_range(0, p - 1);

	MPI_Finalize();
}