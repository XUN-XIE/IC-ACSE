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
                                                                                           
  @file     Cell_Matrix.hpp
  @brief    header file for constructing matrix of cell used for evolution
  @Details.
   basic class method such as constructors, deconstructor, copy constructors,
   copy assignment, move constructors and move assignment.
   Meanwhile, I overload the ostream symbol "<<" for easily printing out matrix of cell.
   Overloading index symbol "()" for easily indexing. Overloading equal compare symbol "==".
   Four methods for evolving matrix of cell.

  @author   XUN XIE
  @email    xx619@ic.ac.uk
  @version  1.0.0.1(版本号)
  @date     20/02/2020
  @license  GNU General Public License (GPL)

*****************************************************************************/

#ifndef life_game_hpp
#define life_game_hpp

#include <iostream>
#include <algorithm>
#include <vector>
#include <mpi.h>
#include <cmath>
#include <memory>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;


/*
 @brief :class for constructing matrix of cell and basic evloving the matrix without MPI
*/
class Matrix
{
public:
    // brief   :pointer to store the value of matrix
    /* 1 represents live cell and 0 represents dead cell. */
    int * values = nullptr;
    
    // brief   :bool value to represent whether the boundary is periodic
    bool periodic;
    
    // brief   :int value reprensents the the size of row of matrix containing boundary
    int rows;
    
    // brief   :int value reprensents the the size of column of matrix containing boundary
    int cols;
    
    // brief   :bool value reprensents whether the matrix produces values randomly
    bool produce_random;

public:
    //--------------------------constructors------------------------
    
    /*
     @brief    :initial constructor
     assigning values of matrix firstly without boundary denpending on whether it is randomly produced (values are 1 or 0).
     Then assigning values of boundary denpending on whether it is periodic
            
     @param[in] rows:   number of row in matrix wihout boundary
                cols:   number of column in matrix wihout boundary
                status:   deciding whether the matix of cell is periodic
                pro_random:   deciding whether produces the values of matrix randomly
     
     @code
     Matrix(10, 10, false, true) example;
     the example represents a 12 by 12 matrix containing non-periodic boundary and the values are produced randomly
    */
    Matrix(int rows, int cols, bool status = false, bool pro_random = true);
    
    
    /*
     @brief    :deconstructor
    */
    ~Matrix();
    
    
    /*
     @brief    :deep copy constructor
    */
    Matrix(const Matrix & A);
    
    
    /*
     @brief    :copy assignment
    */
    Matrix & operator=(const Matrix &A);
    
    
    /*
     @brief     :Move constructor
     */
    Matrix(Matrix && A);
    
    
    /*
     @brief     :Move assignment
     */
    Matrix & operator=(Matrix && A);
    
    
    /*
     @brief    :index symbol
     indexing value of matrix by row position and column position
            
     @param[in] i:   position of row of matrix
                j:   position of column of matrix
    */
    int & operator() (int i, int j);
    
    
    /*
     @brief    :printing out symbol
     printing out the values of matrix without boundary
    */
    friend ostream & operator<<(ostream & output, const Matrix & A);
    
    
    /*
     @brief    :comparing values of two matrix
     only comparing values in matrix wihout boundary
    */
    friend bool operator==(Matrix & A, Matrix & B);
    
    
    //-----------------------------Basic Solver--------------------------
    /*
     @brief    :construct periodic matrix
     if the matrix is periodic, the method will assign values for boundary
    */
    void construct_periodic();
    
    
    /*
     @brief    :basic evolution without periodic
     according to the rule of game of life to update cell of matrix
    */
    void basic_evolution();

    
    /*
     @brief    :general evolution
     according to  periodic to decide which kind of evolving
    */
    void evolution();
    
    
    /*
     @brief    :calculate the sum of values of eight neighboring cells
     
     @param[in] i:   position of row of matrix
                j:   position of column of matrix
    */
    int calculate_neighbor(int i, int j);
};

#endif /* life_game_hpp */
