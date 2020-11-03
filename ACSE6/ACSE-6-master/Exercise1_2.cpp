#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>
using namespace std;

int id, p;
int tag_num = 1;

int main(int argc, char *argv[])
{
	MPI_Status status;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL)+id*100);

	int source = (id + p - 1) % p;
	int destination = (id + 1) % p;
	int num_recv;

	int *data;

	if (id == 0)
	{
		int num_additional = rand() % 3 + 1;
		data = new int[num_additional];
		cout << id << " sent:";
		for (int i = 0; i < num_additional; i++)
		{
			data[i] = rand();
			cout << "\t" << data[i];
		}
		cout << endl;
		MPI_Send(data, num_additional, MPI_INT, destination, tag_num, MPI_COMM_WORLD);
		delete[] data;
		MPI_Probe(source, tag_num, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &num_recv);
		data = new int[num_recv];
		MPI_Recv(data, num_recv, MPI_INT, source, tag_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		cout << "Process "<<id<< " recieved:";
		for (int i = 0; i < num_recv; i++)
			cout << "\t" << data[i];
		cout << endl;
		delete[] data;
	}
	else
	{
		MPI_Probe(source, tag_num, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &num_recv);
		int num_additional = rand() % 3 + 1;
		data = new int[num_additional+num_recv];
		MPI_Recv(data, num_recv, MPI_INT, source, tag_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		cout << id << " sent:";
		for (int i = 0; i < num_additional; i++)
		{
			data[num_recv+i] = rand();
			cout << "\t" << data[num_recv + i];
		}
		cout << endl;
		MPI_Send(data, num_additional + num_recv, MPI_INT, destination, tag_num, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}