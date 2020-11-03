#include "Cell_Matrix.hpp"

//--------------------------constructors------------------------

// Constructor - using an initialisation list here
Matrix::Matrix(int row, int col, bool status, bool pro_random): rows(row + 2), cols(col + 2), periodic(status), produce_random(pro_random)
{
    srand((double)time(0) + rand() % 100000);
    values = new int [rows * cols];
    
    // produce values randomly if produce_random = true
    if(produce_random)
        for( int i = 0; i != rows; i++ )
            for( int j = 0; j != cols; j++ )
                values[i * cols + j] = rand() % 2;
    
    // constructing boundary depending on periodic
    if(periodic)
    {
        construct_periodic();
    }
    else
    {
        for (int i = 0; i != rows; i++)
           {
               (*this)(i,0) = 0;
               (*this)(i,cols-1) = 0;
           }
           for (int i = 0; i != cols; i++)
           {
               (*this)(0,i) = 0;
               (*this)(rows-1,i) = 0;
           }
    }
}

// destructor
Matrix::~Matrix()
{
    delete [] values;
}

// define copy constructor and copy assighment because of
// using dynamical memory management (namely NEW)

// Copy constructor − creates Matrix with the same entries as input, A
Matrix::Matrix(const Matrix & A)
{
    rows = A.rows; // Matrix dimension fields
    cols = A.cols;
    periodic = A.periodic;
    produce_random = A.produce_random;

    values = new int [A.rows * A.cols];

    for (int i=0; i < A.rows; i++){
        for (int j=0; j < A.cols; j++)
            // Copy across the entries from Matrix A
            values[i * cols + j] = A.values[i * cols + j];
    }
}


Matrix & Matrix::operator=(const Matrix & A)
{
    // Destruct previous entries
    delete [] values;
    // Assign new dimensions to be equal to that of the RHS
    rows = A.rows;
    cols = A.cols;
    periodic = A.periodic;
    produce_random = A.produce_random;

    // Allocate the memory as in the constructor
    values = new int [A.rows * A.cols];

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
Matrix::Matrix(Matrix && A) : rows{A.rows}, cols{A.cols}, periodic{A.periodic}, produce_random(A.produce_random)
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

Matrix & Matrix::operator=(Matrix && A)
{
    if (this == &A)
        return *this;
        
    delete [] values;

    values = A.values;
    rows = A.rows;
    cols = A.cols;
    periodic = A.periodic;
    produce_random = A.produce_random;

    A.rows = 0;
    A.cols = 0;
    A.values = nullptr;

    return *this;
}

// indexing value of matrix by row position and column position
int & Matrix::operator() (int i, int j)
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

// Overloads the '<<' operator to allow easy printing of matrices
ostream & operator<<(ostream & output, const Matrix & A)
{
    for (int i=1; i<A.rows-1; i++)
    {
        for (int j=1; j < A.cols-1; j++)
            output << A.values[i * A.cols + j] << '\t';
        output << "\n";
    }
    output << "\n";
    
    return output;
}

// Overloads the '==' operator to allow decide whether the two matrix is equal.
// only compare the values in core part.
bool operator==(Matrix & A, Matrix & B)
{
    if ( A.rows != B.rows || A.cols != B.cols)
        return false;
    
    if ( A.periodic != B.periodic )
        return false;
    
    for (int i = 1; i != A.rows - 1; i++)
        for (int j = 1; j != A.cols - 1; j++)
            if ( A(i,j) != B(i,j) )
                return false;
    
    return true;
}

/*
calculate the summation of eight points around a point
*/
int Matrix::calculate_neighbor(int i, int j)
{
    int sum = 0;
    for ( int m = -1; m != 2; m++ )
        for ( int n = -1; n != 2; n++ )
            if ( values[ ( i + m ) * cols + j + n ] > 0 )
                sum += 1;
    
    if ( values[ i * cols + j ] )
        sum -= 1;

    return sum;
}


// construct the boundary values for ghost condition
void Matrix::construct_periodic()
{
    int i, j;
    for ( i = 1; i != rows - 1; i++ )
    {
        values[ i * cols ] = values[ ( i + 1 ) * cols - 2 ];
        values[ ( i + 1 ) * cols - 1 ] = values[ i * cols + 1 ];
    }
    
    for ( j = 1; j != cols - 1; j++ )
    {
        values[ j ] = values[ ( rows - 2 ) * cols + j];
        values[ ( rows - 1 ) * cols + j ] = values[ cols + j ];
    }
    
    (*this)(0,0) = (*this)(rows-2, cols-2);
    (*this)(0,cols-1) = (*this)(rows-2, 1);
    (*this)(rows-1,0) = (*this)(1, cols-2);
    (*this)(rows-1,cols-1) = (*this)(1, 1);
}

// basic evolution without periodic
void Matrix::basic_evolution()
{
    Matrix copy(*this);
    
    int cnt;
    for( int i = 1; i != rows - 1; i++ )
        for( int j = 1; j != cols - 1; j++ )
        {
            cnt = copy.calculate_neighbor(i, j);
            if ( copy(i, j) == 0)
            {
                if ( cnt == 3 )
                    values[ i * cols + j ] = 1;
            }
            else
            {
                if ( cnt != 2 && cnt != 3 )
                    values[ i * cols + j ] = 0;
            }
        }
}

// general evolution
void Matrix::evolution()
{
    if (periodic)
        this->construct_periodic();
    this->basic_evolution();
    
    if (periodic)
        this->construct_periodic();
}
