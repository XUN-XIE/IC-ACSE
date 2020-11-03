#include <mpi.h>
#include <iostream>
using namespace std;

int id, p;
int tag_num = 1;

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


my_class my_data[10];
int value_top, value_bottom;

void Send_Data()
{
	int block_lengths[3];
	MPI_Aint addresses[3];
	MPI_Datatype typelist[3];

	MPI_Datatype MPI_Temp;

	typelist[0] = MPI_INT;
	block_lengths[0] = 1;
	MPI_Get_address(&value_top, &addresses[0]);

	typelist[1] = MPI_INT;
	block_lengths[1] = 1;
	MPI_Get_address(&value_bottom, &addresses[1]);

	typelist[2] = my_class::MPI_type;
	block_lengths[2] = 10;
	MPI_Get_address(my_data, &addresses[2]);

	MPI_Type_create_struct(3, block_lengths, addresses, typelist, &MPI_Temp);
	MPI_Type_commit(&MPI_Temp);

	MPI_Bcast(MPI_BOTTOM, 1, MPI_Temp, 0, MPI_COMM_WORLD);

	MPI_Type_free(&MPI_Temp);
}


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	my_class::buildMPIType();


	if (id == 0)
	{
		for (int i = 0; i < 10; i++)
		{
			my_data[i].I1 = 6+i*25;
			my_data[i].I2 = 3-i*4;
			my_data[i].D1 = 10.0+31.*i;
			my_data[i].var_not_to_send = 25;
			strncpy(my_data[i].S1, "My test string", 50);
		}

		value_top = 16;
		value_bottom = 5;
	}

	Send_Data();

	for (int i = 0; i < 10; i++)
		cout << "Process: " << id << "\t" << i << ": I1=" << my_data[i].I1 << " I2=" << my_data[i].I2 << " D1= " << my_data[i].D1 << " S1=" << my_data[i].S1 << ". The unsent variable is " << my_data[i].var_not_to_send << endl;
	cout << "Process: " << id << "\ttop value: " << value_top << "\tbottom value: " << value_bottom << endl;

	MPI_Type_free(&my_class::MPI_type);
	MPI_Finalize();
}