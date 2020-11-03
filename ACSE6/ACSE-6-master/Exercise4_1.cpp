#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <vector>

using namespace std;

int id, p, tag_num = 1;

MPI_Datatype MPI_Particle;

class CParticle
{
public:
	double x[2];
	double v[2];

	static void buildMPIType();
};

double Random()
{
	//+1 on RAND_MAX to ensure that the returned value is strictly less than 1
	return (double)rand() / ((double) RAND_MAX + 1.0);
}

void CParticle::buildMPIType()
{
	int block_lengths[2];
	MPI_Aint offsets[2];
	MPI_Aint addresses[2], add_start;
	MPI_Datatype typelist[2];

	CParticle temp;

	typelist[0] = MPI_DOUBLE;
	block_lengths[0] = 2;
	MPI_Get_address(temp.x, &addresses[0]);

	typelist[1] = MPI_DOUBLE;
	block_lengths[1] = 2;
	MPI_Get_address(temp.v, &addresses[1]);

	MPI_Get_address(&temp, &add_start);
	for (int i = 0; i < 2; i++) offsets[i] = addresses[i] - add_start;

	MPI_Type_create_struct(2, block_lengths, offsets, typelist, &MPI_Particle);
	MPI_Type_commit(&MPI_Particle);
}

CParticle *full_particle_list = nullptr;
vector<CParticle> proc_particle_list;
int num_particle_total = 10000;

double domain_size[2] = { 1.0, 1.0 }, max_vel = 1.0;

int proc_from_x(double x)
{
	return (int) ((p*x) / domain_size[0]);
}

void distribute_particles(void)
{
	if (id == 0)
	{
		for (int i = 0; i < num_particle_total; i++)
		{
			int send_proc = proc_from_x(full_particle_list[i].x[0]);

			if (send_proc == id)
				proc_particle_list.push_back(full_particle_list[i]);
			else
				MPI_Send(&full_particle_list[i], 1, MPI_Particle, send_proc, tag_num, MPI_COMM_WORLD);
		}

		for (int i = 1; i < p; i++)
		{
			MPI_Send(nullptr, 0, MPI_Particle, i, tag_num, MPI_COMM_WORLD);
		}
	}
	else
	{
		MPI_Status status;
		do
		{
			int count;

			CParticle temp;
			MPI_Recv(&temp, 1, MPI_Particle, 0, tag_num, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_Particle, &count);

			if (count == 0)
				break;
			else proc_particle_list.push_back(temp);

		} while (true);
	}
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 1000);

	CParticle::buildMPIType();

	if (id == 0)
	{
		full_particle_list = new CParticle[num_particle_total];

		for (int i = 0; i < num_particle_total; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				full_particle_list[i].x[j] = Random()*domain_size[j];
				full_particle_list[i].v[j] = Random()*max_vel;
			}
		}
	}

	distribute_particles();

	cout << "Process " << id << " received " << proc_particle_list.size() << " particles" << endl;

	MPI_Type_free(&MPI_Particle);
	MPI_Finalize();

	delete[] full_particle_list;
}
