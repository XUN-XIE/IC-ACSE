#ifndef Matrix_h
#define Matrix_h

#include <fstream>
#include <time.h>
#include <math.h>
#include <string>
#include <iostream>
#include <cassert>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdlib>

using std::ostream;
using std::vector;
using std::cout;
using std::endl;
using std::cerr;
using std::move;
using std::swap;
using std::max;

template <class T>
class Matrix
{
public:
    // ------------------------built-in data------------------------
    // ------------------------built-in data------------------------
    T * values = nullptr;
    int rows = -1;
    int cols = -1;

    //--------------------------constructors------------------------
    // constructor where we want to preallocate ourselves
    Matrix(int rows, int cols, bool preallocate);
    // constructor where we already have allocated memory outside
    Matrix(int rows, int cols, T * values_ptr): rows(rows), cols(cols), size_of_values(rows * cols), values(values_ptr) {}
    // Creates Matrix of given dimension
    Matrix(int no_of_rows,int no_of_columns);
    
    // destructor
    virtual ~Matrix();
    
    // deep copy constructor
    Matrix(const Matrix<T> & A);
    // copy assignment
    Matrix<T> & operator=(const Matrix<T> &A);
    // Move constructor
    Matrix(Matrix<T> && A);
    // Move assignment
    Matrix<T> & operator=(Matrix<T> && A);
    
    // Overloads (), so A(i,j) returns the i,j entry a la MATLAB
    T & operator() (int i, int j);

    // ------------------------Binary Operators------------------------
    template <class U>
    friend Matrix<U> operator+(const Matrix<U> & A, const Matrix<U> & B);
    template <class U>
    friend Matrix<U> operator-(const Matrix<U> & A, const Matrix<U> & B);
    template <class U>
    friend Matrix<U> operator*(const U & p, const Matrix<U> & A);
    template <class U>
    friend Matrix<U> operator*(const Matrix<U> & A, const U & p);
    template <class U>
    friend Matrix<U> operator*(const Matrix<U> & A, const Matrix<U> & B);
    template <class U>
    friend vector<U> operator*(const Matrix<U> & A, const vector<U> & x);
    template <class U>
    friend vector<U> operator*(const vector<U> & x, const Matrix<U> & A) { return A * x; }
    template <class U>
    friend Matrix<U> operator/(const Matrix<U> & A, const U & p);
    template <class U>
    friend Matrix<U> operator/(const Matrix<U> & b, const Matrix<U> & A);

    // Overload ~ to mean transpose
    template <class U>
    friend Matrix<U> operator~(const Matrix<U> & A);
    
    
    // ------------------------Functions that are friends------------------------
    // Overloads the '<<' operator to allow easy printing of matrices
    template <class U>
    friend ostream & operator<<(ostream & output, const Matrix<U> & A);

    // Create nxn Identity Matrix
    Matrix<T> eye(int n) const;
    
    // detect whether the Matrix is square
    friend void square_(Matrix & A) { if (A.rows != A.cols ) {cerr << "Not a square"; exit(0);} }

    
// We want our subclass to know about this
protected:
   bool preallocated = false;

// Private variables - there is no need for other classes
// to know about these variables
private:
   int size_of_values = -1;
};

template <class T>
ostream & operator<<(ostream & os, vector<T> & vec)
{
    for (auto x: vec)
        os << x << "\t";
    os << endl;
    return os;
}

// find the index of maximum value in a vector
template <class U>
int argmax(vector<U> & vec);

// define the operator minus between vector.
template <class T>
vector<T> operator-(const vector<T> & a, const vector<T> & b);
#endif /* Matrix_h */
