#ifndef CSRMatrix_h
#define CSRMatrix_h

#include "Matrix.h"

template <class T>
class CSRMatrix: public Matrix<T>
{
public:
    // Explicitly using the C++11 nullptr here
    int *row_position = nullptr;
    int *col_index = nullptr;

    // How many non-zero entries we have in the matrix
    int nnzs=-1;

    // constructor where we want to preallocate ourselves
    CSRMatrix(int rows, int cols, int nnzs, bool preallocate);
    // constructor where we already have allocated memory outside
    CSRMatrix(int rows, int cols, int nnzs, T *values_ptr, int *row_position, int *col_index);
    // destructor
    ~CSRMatrix();

    // deep copy constructor
    CSRMatrix(const CSRMatrix<T> & A);
    // copy assignment
    CSRMatrix<T> & operator=(const CSRMatrix<T> &v);

    // Move constructor
    CSRMatrix(CSRMatrix<T> && A);
    // Move assignment
    CSRMatrix<T> & operator=(CSRMatrix<T> && A);


    // Print out the values in our matrix
    virtual void printMatrix();

    // Perform some operations with our matrix
    void matMatMult(CSRMatrix<T>& mat_right, CSRMatrix<T>& output);
    // Perform some operations with our matrix
    void matVecMult(double *input, double *output);

    template <class U>
    friend CSRMatrix<U> operator~(const CSRMatrix<U> & A);
// Private variables - there is no need for other classes
// to know about these variables
private:
   
};


#endif /* CSRMatrix_h */
