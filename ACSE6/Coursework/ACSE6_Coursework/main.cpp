#include "Evolution_Solver.hpp"

int main(int argc, char *argv[])
{
    int size = 30;
    int loopnum = 30;

    grid test_grid_non(size, size, false, false);
    grid test_grid_ghost(size, size, true, false);
    
    stripe test_stripe_non(size, size, false, false);
    stripe test_stripe_ghost(size, size, true, false);

    int myid, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    
    test_grid_non.grid_solver(loopnum, myid, numprocs, false, true);
    test_grid_ghost.grid_solver(loopnum, myid, numprocs, false, true);
    
    test_stripe_non.striped_solver(loopnum, myid, numprocs,false, true);
    test_stripe_ghost.striped_solver(loopnum, myid, numprocs, false, true);

    MPI_Finalize();
}
