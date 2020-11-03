# ACSE-5- C++ Punisher

Implement an algorithm to solve the linear system Ax=b, where A is a positive definite matrix, and x and b. The program which implements this will build upon the Matrix.cpp and Matrix.h libraries that we have constructed in class (as such A or A -1 must be of type Matrix, or a class derived from this, such as CSRMatrix, or your own custom subtype).

Compressed Sparse Row is a matrix storaging technique that only cares about non-zero values in the matrix and, for that reason, it is more memory efficient than regular matrix storaging costs. Instead of storaging <a href="https://www.codecogs.com/eqnedit.php?latex=n^{2}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?n^{2}" title="n^{2}" /></a> elements, CSR format storages <a href="https://www.codecogs.com/eqnedit.php?latex=2nnz&space;&plus;&space;n&space;&plus;&space;1" target="_blank"><img src="https://latex.codecogs.com/gif.latex?2nnz&space;&plus;&space;n&space;&plus;&space;1" title="2nnz + n + 1" /></a> elements, where <a href="https://www.codecogs.com/eqnedit.php?latex=nnz" target="_blank"><img src="https://latex.codecogs.com/gif.latex?nnz" title="nnz" /></a> equals the number of non-zero elements in the matrix.

A simple explanation on the CSR format can be found in [netlib.org](http://netlib.org/linalg/html_templates/node91.html).

A nice collection of .mtx format matrices can be seen in [SuiteSparse Matrix Collection](https://sparse.tamu.edu/).

## Installation Guide

To install the module `acse-5-assignment-c-punisher
` clone the respository to your computer with 
``
git clone https://github.com/acse-2019/acse-5-assignment-c-punisher.git
``
Then change directory into the root of the repo you just cloned 
``
cd acse-5-assignment-c-punisher
``
and the run 
```
gcc main.cpp -o main.out  

./main.out
```

## User instructions

Save 
-- [main.cpp]
-- [CSRMatrix.cpp](CSRMatrix.h)
-- [Solver.cpp](Solver.h)
-- [Interface.cpp](Interface.h)
to your code directory and inform it as a header in your file.

```C++
#include <iostream>
#include <ctime>
#include <fstream>
#include "Matrix.cpp"
#include "Solver.cpp"
#include "CSRMatrix.cpp"
#include "Interface.h"
```

When dealing with a main, which will call Interface function:
```C++
#include <fstream>
#include "Matrix.h"
#include "Solver.h"
#include "CSRMatrix.h"
#include "Interface.h"

## Test

int main(int argc, const char * argv[])
{
    /* 
       include all linear sovler function (UI)
       The first Interface is class, and than,
       create the object interface to call 
       non-paramaters constructor to initialize
    */
    Interface interface;
}
```
This is Inteface initialize screen:
```C++
void Interface::interfaceIntro()
{
    /*
        C++ Punisher Introduce our terminal interface
     */
    cout << endl;
    cout << " ----------------------------------" << endl;
    cout << "|    MATRIX LINEAR SOLVER SYSTEM   |" << endl;
    cout << " ----------------------------------" << endl;
    cout << endl;
    cout << " ----------------------------------" << endl;
    cout << "| Developed by :                   |" << endl;
    cout << "|                                  |" << endl;
    cout << "|           CHAO-LUN LIU           |" << endl;
    cout << "|           JIABO WANG             |" << endl;
    cout << "|           XUN XIE                |" << endl;
    cout << " ----------------------------------" << endl;
    cout << endl;
    cout << " ------------------------------------------------" << endl;
    cout << "| This linear solver is designed to solve Ax = b,|" << endl;
    cout << "| trying to  find x                              |" << endl;
    cout << " ------------------------------------------------" << endl;
    cout << endl;
    cout << " Data Used:" << endl;
    cout << " -------------------------------------------------" << endl;
    cout << "| Data is radom to generate from -100 - 100:      |" << endl;
    cout << "| Matrix size is larger than 10 x 10              |" << endl;
    cout << "| There have 3 different categories of matrix     |" << endl;
    cout << " -------------------------------------------------" << endl;
    cout << endl;
    cout << " ----------------------------------" << endl;
    cout << "|        1. Dense matrix           |" << endl;
    cout << "|        2. Sparse matrix          |" << endl;
    cout << " ----------------------------------" << endl;
    cout << endl;
    cout << "------------------------------" << endl;
    cout << " Would you like to continue? (y/n)" << endl;
    cout << " >> ";
}

```

And then, you can choose type of matrix:

```C++

cout << " ----------------------------------" << endl;
cout << "|     Select the type of matrix    |:" << endl;
cout << " ----------------------------------" << endl;
cout << endl;
cout << " -----------------------------------------------" << endl;
cout << "| 1: Dense Matrix (all values >= 1)             |" << endl;
cout << "| 2: Sparse Matrix (more than half values == 0) |" << endl;
cout << "| b: Back                                       |" << endl;
cout << "| x: Exit                                       |" << endl;
cout << " -----------------------------------------------" << endl;
cout << " >> ";

```

Next, choosing your matrix size:

```C++
cout << endl;
cout << " ----------------------------------" << endl;
cout << "|     Select the size of matrix    |:" << endl;
cout << " ----------------------------------" << endl;
cout << endl;
cout << " -----------------------------------------------" << endl;
cout << "| 1: 10 x 10                                    |" << endl;
cout << "| 2: 15 x 15                                    |" << endl;
cout << "| 3: 20 x 20                                    |" << endl;
cout << "| b: Back                                       |" << endl;
cout << "| x: Exit                                       |" << endl;
cout << " -----------------------------------------------" << endl;
cout << " >> ";
```

After that, the interface will give you the opportunity to change random b:
```C++
cout << " ------------------------------------------" << endl;
cout << "|    Do you want to change random matrix?  |:" << endl;
cout << " ------------------------------------------" << endl;
cout << endl;
cout << " --------------------------------" << endl;
cout << "| a: Yes                         |" << endl;
cout << "| b: No                          |" << endl;
cout << " --------------------------------" << endl;
cout << " >> ";
```
Finally, users can select the algorithm to solve linear problem (Ax = b) to get x:
```C++
cout << " ------------------------------------------" << endl;
cout << "|  Select Algorihm to solve dense matrix   |:" << endl;
cout << " ------------------------------------------" << endl;
cout << endl;
cout << " -----------------------------------------------" << endl;
cout << "| 1: Gauss Elimination                          |" << endl;
cout << "| 2: LU factorisation                           |" << endl;
cout << "| 3: Gauss Seidel                               |" << endl;
cout << "| 4: LU pp (without partial pivoting)           |" << endl;
cout << "| 5: Conjugate Gradient                         |" << endl;
cout << "| b: Back                                       |" << endl;
cout << "| x: Exit                                       |" << endl;
cout << " -----------------------------------------------" << endl;
cout << " >> ";
```

Give one example, when user choose dense matrix, 10x10, and Gauss-Jordan:
```C++
A Matrix is (10x10) = {
 76,  2, -31,   5,  -4,  -4,  73,   9, -15, -25,
  2, 84,  -6,  36,  23,  10,  14,  33, -26, -34,
-31, -6,  52,  -5,  -2,   8, -38,  -7,  -9,  12,
  5, 36,  -5,  83,  29, -12, -10,  51,  23,   3,
 -4, 23,  -2,  29,  56,  18, -13,  36,  25,  11,
 -4, 10,   8, -12,  18,  48, -12,  21, -27,   5,
 73, 14, -38, -10, -13, -12,  94, -12, -33, -34,
  9, 33,  -7,  51,  36,  21, -12,  64,  23,   6,
-15,-26,  -9,  23,  25, -27, -33,  23, 106,  26,
-25, -34, 12,   3,  11,   5, -34,   6,  26,  45 };

// b is radom to generate
vector<double> b = rand_produce_b(A,-100,100);
cout << b is random << b << endl;
x = Gauss-Jordan(A, b);
cout << x is solution << endl;
```


## Apply linear Solver

### Initialise the Matrix and b
Firstly, you need to have an array or a vector which stores values of your matrix and by the order from upper left corner of matrix to lower right corner of matrix.

Then, we recommend you to initialise by this way:
```
Matrix<type> A(rows, cols);
for (int i = 0; i != rows * cols; i++)
    A.values[i] = array[i];
```
In this way, you can store your values in int or double type, and safe conversion will happen to ensure the correct assignment to Matrix.

Finally, you can apply the linear solver:
```
vector<type> x;
x = linear_solver(A, b);
```

### Using multiple b
You need to store your b in Matrix form.
```
Matrix<type> b(rows, number_of_b);
```
and you need to initialise b as same step as initialising A.

Then, you can call linear solver as the same function as sole b. (note: there are only three algorithms can apply multiple b: Gaussian Elimination, Gauss Jordan, LU decomposition.)

## Documentation

```

Information on the implementation and functions included are in the ``Report.pdf`` document.

```

## Testing

![Image description](https://github.com/acse-2019/acse-5-assignment-c-punisher/blob/master/png/test_result.png)
