#include <mpi.h>
#include <iostream>

using namespace std;

int id, p;

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	cout << "Processor " << id << " of " << p << endl;
	cout.flush();

	MPI_Barrier(MPI_COMM_WORLD);
	if (id == 0) cout << "Every process has got to this point now!" << endl;

	MPI_Finalize();
}