/*
 CID : 01752815
 email : xx619@ic.ac.uk
 compiler : Xcode
 operating system : MacOS Catalina
 number of logical processors : 6
 
 all test is run by default 12 threads, and unit is second
 Basic test:
 Serial time consuming : 0.454017
 First version parallel:    parallel time consuming is: 0.384429
 Second version parallel:    parallel time consuming is: 0.411009
 
 without outputting file test:
 Serial code without outputting file:    serial time consuming is: 0.194188
 First version parallel without outputting file:        parallel time consuming is: 0.0488532
 Second version parallel without outputting file:    parallel time consuming is: 0.0643501
 
 change size to 1000 * 1000 (without outputting file)
 Serial code without outputting file:    serial time consuming is: 18.7649
 First version parallel without outputting file:        parallel time consuming is: 3.19472
 Second version parallel without outputting file:    parallel time consuming is: 4.57573
 */

//-----------------------------------first version(faster one)----------------------------------
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <vector>
#include "omp.h"

using namespace std;

//Note that this is a serial implementation with a periodic grid
vector<vector<bool>> grid, new_grid;
int imax, jmax;
int max_steps = 100;

int num_neighbours(int ii, int jj)
{
    int ix, jx;
    int cnt = 0;
// do not add parallel for here because the loop is only eight times
// so the cost of add parallel for is more than efficiency carried by parallel for
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
            if (i != 0 || j != 0)
            {
                ix = (i + ii + imax) % imax;
                jx = (j + jj + jmax) % jmax;
                if (grid[ix][jx]) cnt++;
            }
    return cnt;
}

void grid_to_file(int it)
{
    stringstream fname;
    fstream f1;

    fname << "output" << "_" << it << ".dat";
    f1.open(fname.str().c_str(), ios_base::out);
// do not add parallel for here because the order of running parallel cannot ensure the output is ordered
    for (int i = 0; i < imax; i++)
    {
        for (int j = 0; j < jmax; j++)
            f1 << grid[i][j] << "\t";
        f1 << endl;
    }
    f1.close();
}

void do_iteration(void)
{
// compute num_neighbours is independt with respect to position
// according to new and old matrix, so old matrix will not change.
// so we can use parallel for to spped up
#pragma omp parallel for
    for (int i = 0; i < imax; i++)
        #pragma omp parallel for
        for (int j = 0; j < jmax; j++)
        {
            new_grid[i][j] = grid[i][j];
            int num_n = num_neighbours(i, j);
            if (grid[i][j])
            {
                if (num_n != 2 && num_n != 3)
                    new_grid[i][j] = false;
            }
            else if (num_n == 3) new_grid[i][j] = true;
        }
    grid.swap(new_grid);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    imax = 100;
    jmax = 100;

    grid.resize(imax, vector<bool>(jmax));
    new_grid.resize(imax, vector<bool>(jmax));

// assignment to different position is independent, so we can add parallel for
#pragma omp parallel for
    for (int i = 0; i < imax; i++)
        for (int j = 0; j < jmax; j++)
            grid[i][j] = (rand() % 2);

// do not add parallel for here because the order of running parallel cannot ensure the grid is updated orderly
    double start, end;
    start = omp_get_wtime();
    for (int n = 0; n < max_steps; n++)
    {
        cout << "it: " << n << endl;
        do_iteration();
        grid_to_file(n);
    }
    end = omp_get_wtime();
    cout << "parallel time consuming is : " << end - start << endl;

    return 0;
}











//-----------------------------------second version(slower one)----------------------------------
//#include <iostream>
//#include <sstream>
//#include <fstream>
//#include <cstdlib>
//#include <time.h>
//#include <vector>
//#include "omp.h"
//
//using namespace std;
//
////Note that this is a serial implementation with a periodic grid
//vector<vector<bool>> grid, new_grid;
//int imax, jmax;
//int max_steps = 100;
//
//int num_neighbours(int ii, int jj)
//{
//    int ix, jx;
//    int cnt = 0;
//// do not add parallel for here because the loop is only eight times
//// so the cost of add parallel for is more than efficiency carried by parallel for
//    for (int i = -1; i <= 1; i++)
//        for (int j = -1; j <= 1; j++)
//            if (i != 0 || j != 0)
//            {
//                ix = (i + ii + imax) % imax;
//                jx = (j + jj + jmax) % jmax;
//                if (grid[ix][jx]) cnt++;
//            }
//    return cnt;
//}
//
//void grid_to_file(int it)
//{
//    stringstream fname;
//    fstream f1;
//
////    fname << "output" << "_" << it << ".dat";
//    fname << "output" << "_" << it << ".txt";
//
//    f1.open(fname.str().c_str(), ios_base::out);
//// do not add parallel for here because the order of running parallel cannot ensure the output is ordered
//    for (int i = 0; i < imax; i++)
//    {
//        for (int j = 0; j < jmax; j++)
//            f1 << grid[i][j] << "\t";
//        f1 << endl;
//    }
//    f1.close();
//}
//
//void do_iteration(void)
//{
//// compute num_neighbours is independt with respect to position
//// according to new and old matrix, so old matrix will not change.
//// so we can use parallel for to spped up
//
//    int all_num = imax * jmax;
//    int i, j;
//#pragma omp parallel for
//    for (int k = 0; k < all_num; k++)
//    {
//        i = k / jmax;
//        j = k % jmax;
//        new_grid[i][j] = grid[i][j];
//        int num_n = num_neighbours(i, j);
//        if (grid[i][j])
//        {
//            if (num_n != 2 && num_n != 3)
//                new_grid[i][j] = false;
//        }
//        else if (num_n == 3) new_grid[i][j] = true;
//    }
//    grid.swap(new_grid);
//}
//
//int main(int argc, char *argv[])
//{
//    srand(time(NULL));
//    imax = 100;
//    jmax = 100;
////    imax = 5;
////    jmax = 5;
//    grid.resize(imax, vector<bool>(jmax));
//    new_grid.resize(imax, vector<bool>(jmax));
//
//    cout << "number of all threads : " << omp_get_max_threads() << endl;
//
//// assignment to different position is independent, so we can add parallel for
//#pragma omp parallel for
//    for (int i = 0; i < imax; i++)
//        for (int j = 0; j < jmax; j++)
//            grid[i][j] = (rand() % 2);
//
//// do not add parallel for here because the order of running parallel cannot ensure the grid is updated orderly
//    double start, end;
//    start = omp_get_wtime();
//    for (int n = 0; n < max_steps; n++)
//    {
//        cout << "it: " << n << endl;
//        do_iteration();
//        grid_to_file(n);
//    }
//    end = omp_get_wtime();
//    cout << "parallel time consuming is : " << end - start << endl;
//
//    return 0;
//}
//
