#include <mpi.h>
#include <iostream>
#include <locale>

using namespace std;

int id, p;

class my_class
{
public:
	int I1, I2;
	int var_not_to_send;
	double D1;
	char S1[50];

	static void buildMPIType();
	static MPI_Datatype MPI_type;
};

MPI_Datatype my_class::MPI_type;

void my_class::buildMPIType()
{
	int block_lengths[4];
	MPI_Aint displacements[4];
	MPI_Aint addresses[4], add_start;
	MPI_Datatype typelist[4];

	my_class temp;

	typelist[0] = MPI_INT;
	block_lengths[0] = 1;
	MPI_Get_address(&temp.I1, &addresses[0]);

	typelist[1] = MPI_INT;
	block_lengths[1] = 1;
	MPI_Get_address(&temp.I2, &addresses[1]);

	typelist[2] = MPI_DOUBLE;
	block_lengths[2] = 1;
	MPI_Get_address(&temp.D1, &addresses[2]);

	typelist[3] = MPI_CHAR;
	block_lengths[3] = 50;
	MPI_Get_address(&temp.S1, &addresses[3]);

	MPI_Get_address(&temp, &add_start);
	for (int i = 0; i < 4; i++) displacements[i] = addresses[i] - add_start;

	MPI_Type_create_struct(4, block_lengths, displacements, typelist, &MPI_type);
	MPI_Type_commit(&MPI_type);
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	my_class::buildMPIType();

	my_class data;

	if (id == 0)
	{
		data.I1 = 6;
		data.I2 = 3.0;
		data.D1 = 10.0;
		data.var_not_to_send = 25;
		strncpy(data.S1, "My test string", 50);
	}

	MPI_Bcast(&data, 1, my_class::MPI_type, 0, MPI_COMM_WORLD);

	cout << "On process " << id << " I1=" << data.I1 << " I2=" << data.I2 << " D1= " << data.D1 << " S1=" << data.S1 << ". The unsent variable is " << data.var_not_to_send << endl;

	MPI_Type_free(&my_class::MPI_type);
	MPI_Finalize();
}