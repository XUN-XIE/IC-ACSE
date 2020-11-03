#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

int id, p;
int tag_num = 1;

void Send_Random_Data(void)
{
	int to_proc;
	while ((to_proc = rand() % p) == id);	//Stop code sending to itself
	int send_data = id;

	MPI_Send(&send_data, 1, MPI_INT, to_proc, tag_num, MPI_COMM_WORLD);

	cout << "Processor " << id << " sent data to processor " << to_proc << endl;
	cout.flush();
}

void Recv_Data(void)
{
	int from_proc;
	int recv_data;
	MPI_Status status;

	while (MPI_Probe(MPI_ANY_SOURCE, tag_num, MPI_COMM_WORLD, &status) != MPI_SUCCESS)
		cout << "Communication timed out or failed! " << id << endl;

	from_proc = status.MPI_SOURCE;

	MPI_Recv(&recv_data, 1, MPI_INT, from_proc, tag_num, MPI_COMM_WORLD, &status);

	cout << "Processor " << id << " received data for processor " << from_proc << endl;
	cout.flush();
}

int main(int argc, char *argv[])
{
	int recv_cnt = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	if (id == 0) Send_Random_Data();

	while (true)
	{
		Recv_Data();
		recv_cnt++;
		if (recv_cnt<10) 
			Send_Random_Data();
		else break;
	}

	cout << endl << "Process " << id << " has received 10 communications and is aborting (the exit will be messy!)" << endl;
	cout.flush();

	MPI_Abort(MPI_COMM_WORLD, 0);
}