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

	MPI_Request* request = new MPI_Request[(p - 1)*2];
	double *send_data = new double[p];
	double *recv_data = new double[p];

	int cnt = 0;

	for (int i=0;i<p;i++)
		if (i != id)
		{
			MPI_Irecv(&recv_data[i], 1, MPI_DOUBLE, i, tag_num, MPI_COMM_WORLD, &request[cnt]);
			cnt++;
		}
		else recv_data[i] = 0;

	for (int i = 0; i<p; i++)
		if (i != id)
		{
			send_data[i] = (double)id / (double)p;
			MPI_Isend(&send_data[i], 1, MPI_DOUBLE, i, tag_num, MPI_COMM_WORLD, &request[cnt]);
			cnt++;
		}
		else send_data[i] = 0;

	MPI_Waitall(cnt, request, MPI_STATUS_IGNORE);

	for (int i = 0; i < p; i++)
		cout << "Processor " << id << " recieved " << recv_data[i] << " from processor " << i << endl;

	delete[] request;
	delete[] recv_data;
	delete[] send_data;

	MPI_Finalize();
}