/* ************************************************************************
  Game of life Basic tool library
                                                                                                  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 3 as
  published by the Free Software Foundation.
                                                                            
  You should have received a copy of the GNU General Public License
  along with OST. If not, see <http://www.gnu.org/licenses/>.
                                                                            
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
                                                                                           
  @file     Evolution_Solver.hpp
  @brief    header file for updating matrix of cell applying MPI
  @Details.
   two classes inherited from Matrix class to update matrix. One class applies stripe decomposition stategy, and another applies grid decomposition stategy

  @author   XUN XIE
  @email    xx619@ic.ac.uk
  @version  1.0.0.1(版本号)
  @date     20/02/2020
  @license  GNU General Public License (GPL)

*****************************************************************************
*/
#ifndef Evolution_Solver_hpp
#define Evolution_Solver_hpp

#include "Cell_Matrix.hpp"

/*
 @brief :class inherited from Matrix class to update matrix applying stripe decomposition stategy
*/
class stripe : public Matrix
{
public:
    // brief   :the number of row of matrix in each process excepting last processor
    int all_except_last;
    
    // brief   :the number of row of matrix in last processor
    int last_num;
    
public:
    /*
     @brief    :initial constructor
     assigning values of matrix firstly without boundary denpending on whether it is randomly produced (values are 1 or 0).
     Then assigning values of boundary denpending on whether it is periodic
    
     @param[in] rows:           number of row in matrix wihout boundary
                cols:           number of column in matrix wihout boundary
                status:         deciding whether the matix of cell is periodic
                pro_random:     deciding whether produces the values of matrix randomly
     
     @code
     stripe(10, 10, false, true) example;
     the example represents a 12 by 12 matrix containing non-periodic boundary and the values are produced randomly
    */
    stripe(int row, int col, bool status = false, bool pro_random = true) : Matrix(row, col, status, pro_random) {}
    
    
    /*
     @brief    :matrix assignment constructor
     assigning values of matrix by input matrix
            
     @param[in] matrix:     input matrix
     
     @code
     Matrix(10, 10, false, true) example;
     stripe(example) example2;
     the example2 is the same as example
    */
    stripe(Matrix & M) : Matrix(M.rows-2, M.cols-2, M.periodic, M.produce_random) {}
    
    //-----------------------------MPI Striped Solver--------------------------
    /*
     @brief    :stripe solver to update matrix applying stripe decomposition strategy
            
     @param[in] loop_num:       represents the number of times to update the matrix
                myid:           the rank of the processor
                numprocs:       the number of all processors
                outfile:        bool value deciding whether output file
                outtime:        bool value deciding whether print out time consuming
     
     @code
     stripe(10, 10, false, true) example;
     example.striped_solver(10, myid, numprocs);
     the example is innitialised and updates 10 times. And it will not output file and time consuming
    */
    void striped_solver(int loop_num, int myid, int numprocs, bool outfile=false, bool outtime=false);
    
    
    /*
     @brief    :stripe decomposition strategy to cut rows of matrix to different size for each processor
            
     @param[in] numprocs:       the number of all processors

     @return:                   a vector stores values which represents the number of row for each processor
     
     @code
     stripe(10, 10, false, true) example;
     example.cut_matrix(4);
     the 10 rows are decomposed to [2, 2, 2, 4] for each processor
    */
    vector<int> cut_matrix(int numprocs);
    
    
    /*
     @brief    :output values of matrix wihout boundary in ".txt" format
     output file for python to create a picture and gif
     
     @param[in] num:                suffixed file name to distinguish different output of update
                runninng_time:      time consuming of an update
                nunmprocs:          number of all processors
                loopnum:            number of update
    */
    void file_out(int num, double running_time, int numprocs, int loopnum=0);
};


/*
 @brief :class inherited from Matrix class to update matrix applying grid decomposition stategy
*/
class grid : public Matrix
{
public:
    // brief   :the number of row of matrix in each process excepting last processor
    int row_all_except_last;
    
    // brief   :the number of column of matrix in each process excepting last processor
    int col_all_except_last;
    
    // brief   :the number of row of matrix in last processor
    int row_last_num;
    
    // brief   :the number of column of matrix in last processor
    int col_last_num;
    
public:
    /*
     @brief    :initial constructor
     assigning values of matrix firstly without boundary denpending on whether it is randomly produced (values are 1 or 0).
     Then assigning values of boundary denpending on whether it is periodic
    
     @param[in] rows:           number of row in matrix wihout boundary
                cols:           number of column in matrix wihout boundary
                status:         deciding whether the matix of cell is periodic
                pro_random:     deciding whether produces the values of matrix randomly
     
     @code
     grid(10, 10, false, true) example;
     the example represents a 12 by 12 matrix containing non-periodic boundary and the values are produced randomly
    */
    grid(int row, int col, bool status = false, bool pro_random = true) : Matrix(row, col, status, pro_random) {}
    
    
    /*
     @brief    :matrix assignment constructor
     assigning values of matrix by input matrix
            
     @param[in] matrix:     input matrix
     
     @code
     Matrix(10, 10, false, true) example;
     grid(example) example2;
     the example2 is the same as example
    */
    grid(Matrix & M) : Matrix(M.rows-2, M.cols-2, M.periodic, M.produce_random) {}
    
    
    //-----------------------------MPI Grid Solver--------------------------
    /*
     @brief    :grid solver to update matrix applying grid decomposition strategy
            
     @param[in] loop_num:       represents the number of times to update the matrix
                myid:           the rank of the processor
                numprocs:       the number of all processors
                outfile:        bool value deciding whether output file
                outtime:        bool value deciding whether print out time consuming
     
     @code
     grid(10, 10, false, true) example;
     example.grid_solver(10, myid, numprocs);
     the example is innitialised and updates 10 times. And it will not output file and time consuming
    */
    void grid_solver(int loopnum, int myid, int numprocs, bool outfile=false, bool outtime=false);
    
    
    /*
     @brief    :grid decomposition strategy to cut rows of matrix to different size for each processor
            
     @param[in] numprocs:       the number of all processors

     @return:                   a vector stores values which represents the number of row for each processor
     
     @code
     grid(10, 10, false, true) example;
     example.cut_matrix(4);
     the 10 rows are decomposed to [2, 2, 2, 4] for each processor
    */
    vector<int> cut_matrix_row(int numprocs);
    
    
    /*
     @brief    :grid decomposition strategy to cut columns of matrix to different size for each processor
            
     @param[in] numprocs:       the number of all processors

     @return:                   a vector stores values which represents the number of column for each processor
     
     @code
     grid(10, 10, false, true) example;
     example.cut_matrix(4);
     the 10 columns are decomposed to [2, 2, 2, 4] for each processor
    */
    vector<int> cut_matrix_col(int numprocs);
    
    
    /*
     @brief    :gather all data in each processor to one processor to construct the whole matrix used for output file
     
     @param[in] myid:               rank of processor
                nunmprocs:          number of all processors
                grid_cols:          the number of column of processor matrix
                row_size:           the number of row of small matrix in processor
                col_size:           the number of column of small matrix in processor
                arr:                small matrix in each processor
    */
    void gather_all_data(int myid, int numprocs, int grid_cols, vector<int> & row_size, vector<int> & col_size, grid & arr);
    
    
    /*
     @brief    :output values of matrix wihout boundary in ".txt" format
     output file for python to create a picture and gif
     
     @param[in] num:                suffixed file name to distinguish different output of update
                runninng_time:      time consuming of an update
                nunmprocs:          number of all processors
                loopnum:            number of update
    */
    void file_out(int num, double running_time, int numprocs, int loopnum = 0);
};

/*
 @brief    :divide p processes into a grid of size m x n and ensure that m and n are integers that are as close to one another as possible
 
 @param[in] nunmprocs:      number of all processors
            myid:           rank of processor
 @param[out] rows:          the row index of processor in processors matrix
            columns:        the column index of processor in processors matrix
*/
void find_dimensions(int numprocs, int myid, int &rows, int &columns);
#endif /* Evolution_Solver_hpp */
