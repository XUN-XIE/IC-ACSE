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
	int *data = new int[20];

	if (id == 0)
		for (int i = 0; i < 20; i++)
			data[i] = rand();

	MPI_Bcast(data, 20, MPI_INT, 0, MPI_COMM_WORLD);

	if (id == 0) cout << "Process 0 sent this data: ";
	else cout << "Process " << id << " received this data: ";

	for (int i = 0; i < 20; i++)
		cout << "\t" << data[i];
	cout << endl;
	cout.flush();
	
	delete[] data;

	MPI_Finalize();
}