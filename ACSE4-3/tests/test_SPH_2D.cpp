#include "SPH_2D.h"
#include "file_writer.h"

SPH_main domain;

int main(void)
{
	domain.set_values();										//Set simulation parameters
	domain.initialise_grid();									//initialise simulation grid

	domain.place_points(domain.min_x,domain.max_x);				//places initial points - will need to be modified to include boundary points and the specifics of where the fluid is in the domain

	domain.allocate_to_grid();									//needs to be called for each time step

	domain.neighbour_iterate(&domain.particle_list[100]);		//finding all the neighbours of the 100th particle in the list - in reality the simulation loop will need to do the calculations for the neighbours of every particle
	
	return 0;
}
