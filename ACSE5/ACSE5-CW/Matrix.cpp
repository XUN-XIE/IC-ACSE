#include "Matrix.h"

//--------------------------constructors------------------------

// Constructor - using an initialisation list here
template <class T>
Matrix<T>::Matrix(int rows, int cols, bool preallocate): rows(rows), cols(cols), size_of_values(rows * cols), preallocated(preallocate)
{
   // If we want to handle memory ourselves
   if (this->preallocated)
      this->values = new T[size_of_values];
}

// destructor
template <class T>
Matrix<T>::~Matrix()
{
   // Delete the values array
   if (this->preallocated){
      delete[] this->values;
   }
}

// Constructor for basic Matrix with specified dimensions
template <class T>
Matrix<T>::Matrix(int no_of_rows, int no_of_columns)
{
    // Matrix dimension fields
    rows = no_of_rows;
    cols = no_of_columns;
    // A is an array of m pointers, each pointing to the
    // first entry in the vector (of length, 'no of cols')
    values = new T [no_of_rows * no_of_columns];
    for (int i=0; i<no_of_rows; i++){
        for (int j=0; j<no_of_columns; j++){
            // Initialise the entries of the Matrix to zero
            values[i * cols + j] = 0.0;
        }
    }
}

// define copy constructor and copy assighment because of
// using dynamical memory management (namely NEW)

// Copy constructor − creates Matrix with the same entries as input, A
template <class T>
Matrix<T>::Matrix(const Matrix<T> & A)
{
    rows = A.rows; // Matrix dimension fields
    cols = A.cols;

    values = new T [A.rows * A.cols];

    for (int i=0; i < A.rows; i++){
        for (int j=0; j < A.cols; j++)
            // Copy across the entries from Matrix A
            values[i * cols + j] = A.values[i * cols + j];
    }
}

template <class T>
Matrix<T> & Matrix<T>::operator=(const Matrix<T> & A)
{
    // Destruct previous entries
    delete [] values;
    // Assign new dimensions to be equal to that of the RHS
    rows = A.rows;
    cols = A.cols;

    // Allocate the memory as in the constructor
    values = new T [A.rows * A.cols];

    // Copy the values across from the RHS
    for (int i=0; i < A.rows; i++)
        for (int j=0; j < A.cols ; j++)
            // Set entries to be the same as the RHS Matrix
            values[i * cols + j] = A.values[i * A.cols + j];

    return *this;
}

// define Move constructor and Move assignment to move object in some cases
// rather than copy object to improve performance

// Move constructor
template <class T>
Matrix<T>::Matrix(Matrix<T> && A) : rows{A.rows}, cols{A.cols}
{
    for (int i=0; i < A.rows; i++)
        for (int j=0; j < A.cols; j++)
            // Copy across the entries from Matrix A
            values[i * cols + j] = A.values[i * cols + j];
        
    A.rows = 0;
    A.cols = 0;

    A.values = nullptr;
}

// Move assighment
template <class T>
Matrix<T> & Matrix<T>::operator=(Matrix<T> && A)
{
    if (this == &A)
        return *this;
        
    delete [] values;

    values = A.values;

    rows = A.rows;
    cols = A.cols;

    A.rows = 0;
    A.cols = 0;

    A.values = nullptr;

    return *this;
}


template <class T>
T & Matrix<T>::operator() (int i, int j)
{
    if (i < 0 || i >= rows)
    {
        cerr << "Row index out of range." << endl;
        exit(0);
    }
    if (j < 0 || j >= cols)
    {
        cerr << "Column index out of range." << endl;
        exit(0);
    }
    return values[i * cols + j];
}
        

// ------------------------Binary Operators------------------------
// Overload the + operator to evaluate: A + B, where A and B are matrices
template <class U>
Matrix<U> operator+(const Matrix<U> & A, const Matrix<U> & B)
{
    int m = A.rows, n = A.cols, p = B.rows, q = B.cols;
    
    if ( m != p || n !=q )
    {
        cerr << "Error: Matrices of different dimensions" << endl;
        exit(0);
    }
    else
    {
    Matrix<U> C(m,n);
    for (int i=0; i<m; i++)
        for (int j=0; j<n; j++)
            C.values[i * A.cols + j]  = A.values[i * A.cols + j] + B.values[i * A.cols + j];
    
    return C;
    }
}

// Overload the − operator to evaluate: A − B, where A and B are matrices
template <class U>
Matrix<U> operator-(const Matrix<U> & A, const Matrix<U> & B){
        int m,n,p,q;
        m = A.rows;
        n = A.cols;
        p = B.rows;
        q = B.cols;

        if ( m != p || n !=q ){
            cout << "Error: Matrices of different dimensions";
            cout << "Returned first argument";
            return A;
        }else{
            Matrix<U> C(m,n);
        for (int i=0; i<m; i++)
            for (int j=0; j<n; j++)
                C.values[i * A.cols + j] =A.values[i * A.cols + j] - B.values[i * A.cols + j];

        return C;
        }
}
        
// Definition of multiplication between a scalar, p and a Matrix, A
template <class U>
Matrix<U> operator*(const U & p, const Matrix<U> & A){
    // Create a Matrix with the same dimensions as A
    Matrix<U> B(A.rows,A.cols);
    for (int i=0; i<A.rows; i++)
        for (int j=0; j<A.cols; j++)
            B.values[i * B.cols + j] = p * A.values[i * A.cols + j]; // Multiply each entry by p

    return B;
}

// Definition of multiplication between a Matrix, A and a scalar, p
template <class U>
Matrix<U> operator*(const Matrix<U> & A, const U & p){
    // Create a Matrix with the same dimensions as A
    Matrix<U> B(A.rows,A.cols);
    for (int i=0; i<A.rows; i++)
        for (int j=0; j<A.cols; j++)
            B.values[i * B.cols + j]= p * A.values[i * A.cols + j]; // Multiply each entry by p

    return B;
}

// Definition of division of a Matrix, A by a scalar, p i.e. A/p
template <class U>
Matrix<U> operator/(const Matrix<U> & A, const U & p){
    // Create a Matrix with the same dimensions as A
    Matrix<U> B(A.rows,A.cols);
    for (int i=0; i<A.rows; i++)
        for (int j=0; j<A.cols; j++)
            B.values[i * A.cols + j] = A.values[i * A.cols + j] / p; // Divide each entry by p

    return B;
}

// Define multiplication for matrices:
template <class U>
Matrix<U> operator*(const Matrix<U> & A, const Matrix<U> & B){
    // Use assertion to check Matrix dimensions are consistent
    assert(A.cols==B.rows);
    // Create a result Matrix, C with the correct dimensions
    Matrix<U> C(A.rows,B.cols);

    U temp = 0;
    // rows (m)
    for (int i=0; i < A.rows ; i++){
        // cols (q)
        for (int j=0; j < B.cols ; j++){
            // dot product step (n sums)
            for (int k=0; k < A.cols ; k++)
                temp += A.values[i * A.cols + k] * B.values[j + k*B.cols];
            // Set the C Matrix values
            C.values[i * A.cols + j] = temp;
            // reset temp
            temp = 0;
        }
    }
    return C;
}

template <class U>
vector<U> operator*(const Matrix<U> & A, const vector<U> & x)
{
    // error handling
    if (A.cols != x.size())
    {
        cerr << "The rank of two Matrix are not the same!";
        exit(0);
    }

    vector<U> result(A.rows);
    U temp = 0;
    for (int i = 0; i < A.rows; i++)
    {
        temp = 0;
        for (int j = 0; j < A.cols; j++)
            temp += A.values[i * A.cols + j] * x[j];
        result[i] = temp;
    }
    
    return result;
}


// Overload the ~ operator to mean transpose
template <class U>
Matrix<U> operator~(const Matrix<U> & A)
{
    // Create a temporary Matrix with reversed dimensions
    Matrix<U> B(A.cols,A.rows);
    // Set the entires of B to be the same as those in A
    for (int i=0; i < A.cols; i++)
        for (int j=0; j < A.rows; j++)
            B.values[i * A.cols + j] = A.values[j * A.cols + i];

    return B;
}

// ------------------------Functions that are friends------------------------
// Overloads the '<<' operator to allow easy printing of matrices
template <class U>
ostream & operator<<(ostream & output, const Matrix<U> & A)
{
    for (int i=0; i<A.rows; i++)
    {
        for (int j=0; j < A.cols; j++)
            output << A.values[i * A.cols + j] << '\t';
        output << "\n";
    }
    output << "\n";
    
    return output;
}

// Create nxn Identity Matrix
template <class T>
Matrix<T> Matrix<T>::eye(int n) const
{
    Matrix<T> E(n,n);
    for (int i = 0; i != n; i++)
        E(i,i) = 1.0;
    return E;
}

// find the index of maximum value in a vector
template <class U>
int argmax(vector<U> & vec)
{
    U max = 0;
    int index = 0;
    for (int i = 0; i != vec.size(); i++)
        if (vec[i] > max)
        {
            max = vec[i];
            index = i;
        }
    
    return index;
}

// define the operator minus between vector.
template <class T>
vector<T> operator-(const vector<T> & a, const vector<T> & b)
{
    assert(a.size() == b.size());
    vector<T> result;
    for (int i = 0; i != a.size(); i++)
        result.push_back(a[i] - b[i]);
    
    return result;
}
