#include <mpi.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <chrono>

#define DO_TIMING

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

#ifdef DO_TIMING
	//The timing starts here - I deliberately exclude the initialisation of MPI, srand and the memory allocation
	//You need to decide what it is that you are timing
	auto start = chrono::high_resolution_clock::now();
#endif

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

#ifndef DO_TIMING
	//Note that I exclude couts when timing the code
	//Things like file writes should also be excluded to get a fairer reading
	for (int i = 0; i < p; i++)
		cout << "Processor " << id << " recieved " << recv_data[i] << " from processor " << i << endl;
#endif

#ifdef DO_TIMING
	//Note that this should be done after a block in case process zero finishes quicker than the others
	//MPI_Waitall is blocking - Otherwise explicitly use MPI_Barrier
	auto finish = chrono::high_resolution_clock::now();
	if (id == 0)
	{
		std::chrono::duration<double> elapsed = finish - start;
		cout << setprecision(5);
		cout << "The code took " << elapsed.count() << "s to run" << endl;
	}
#endif

	delete[] request;
	delete[] recv_data;
	delete[] send_data;

	MPI_Finalize();
}