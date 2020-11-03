
#include "Solver.h"
#include <algorithm>
#include <iomanip>

// produce random value b for given Matrix
// and user can input the rand seed to change the random state.
template <class U>
vector<U> produce_b(const Matrix<U> & A, int min, int max)
{
    int seed = rand() % 10000;
    // set rand seed to determin the random state
    srand(seed);
    vector<U> b(A.rows);
    for (int i = 0; i < A.rows; i++)
        // set max and min to determine the range of value in b
        b[i] = (rand()%(max-min+1)+min);

    return b;
}

// change random value b for given Matrix
// and user can input the rand seed to change the random state.
template <class U>
vector<U> rand_produce_b(const Matrix<U> & A, int min, int max)
{
    // set rand seed to determin the random state (change again)
    srand(time(0));
    vector<U> b(A.rows);
    for (int i = 0; i < A.rows; i++)
        // set max and min to determine the range of value in b
        b[i] = rand()%(max-min+1)+min;

    return b;
}

/*
 This function swaps rows in matrix A
 (and remember that we need to do likewise for the vector b
 we are performing the same operations on)
*/
template <class U>
void swap_row(Matrix<U> & A, int i, int j)
{
    if (i == j)
        return;
    int n;
    for (n = 0; n < A.cols; n++)
        swap(A(i,n), A(j,n));
}

// ------------------------Gauss Family method------------------------
// back and forward subsititution algorithms used for Gauss Elimination, Gauss Jordan, LU decomposition.

/*
Function to perform back subsitution on the system Ax=b.

Returns the solution x.

Assumes that A is on upper triangular form.
*/
template <class U>
vector<U> back_substitution(Matrix<U> & A, vector<U> & b)
{
    // square matrix is cheked by the call function
    int n = A.rows;
    U s;
    vector<U> x(n);
    // start at the end (row n-1) and work backwards
    for (int k = n-1; k != -1; k--)
    {
        s = 0;
        for (int j = k+1; j != n; j++)
            s += A(k,j) * x[j];
        x[k] = (b[k] - s) / A(k,k);
    }
    
    return x;
}

template <class U>
vector<U> forward_substitution(Matrix<U> & A, vector<U> & b)
{
    // square matrix is cheked by the call function
    int n = A.rows;
    U s;
    vector<U> x(n);
    for (int k = 0; k != n; k++)
    {
        s = 0;
        for (int j = 0; j != k; j++)
            s += A(k,j) * x[j];
        x[k] = (b[k] - s) / A(k,k);
    }
    
    return x;
}

// Gaussian elimination method

/*
A function which utilises our upper_triangle and
back_substitution functions to implement Gaussian elimination.

This version takes local copies of A and b so they are not
over-written.
*/
template <class U>
vector<U> GE(const Matrix<U> & tmpA, const vector<U> & tmpb)
{
    // check A is square
    int n = (int) tmpb.size();
    assert(tmpA.rows == tmpA.cols);
    assert(tmpA.rows == n);
    Matrix<U> A(tmpA);
    vector<U> b(tmpb);
    
    // apply function to change original A to upper triangular matrix form, which is used for later back substituting
    upper_triangle(A, b);
    vector<U> x(b.size());
    x = back_substitution(A, b);

    return x;
}

/*
A function to covert A into upper triangluar form through row operations.
The same row operations are performed on the vector b.

Note that this implementation does not use partial pivoting which is introduced below.

Also note that A and b are overwritten, and hence we do not need to return anything
from the function.
*/
template <class U>
void upper_triangle(Matrix<U> & A, vector<U> & b)
{
    // square matrix is cheked by the call function
    int n = (int) b.size();
    U s;
    // Loop over each row below the pivot row, including the last row which we do need to update
    for (int k = 0; k != n-1; k++)
    {
        for ( int i = k+1; i != n; i++)
        {
    // Define the scaling factor for this row outside the innermost loop otherwise
    // its value gets changed as you over-write A!!
    // There's also a performance saving from not recomputing things when not strictly necessary
            s = A(i,k) / A(k,k);
            // Update the current row of A by looping over the column j
            // start the loop from k as we can assume the entries before this are already zero
            for ( int j = k; j != n; j++)
                A(i,j) -= s * A(k,j);
            // and update the corresponding entry of b
            b[i] -= s * b[k];
        }
    }
}

// Gauss-Jordan elimination
template <class U>
vector<U> Gauss_Jordan(const Matrix<U> & tmpA, const vector<U> & tmpb)
{
    // check A is square
    int n = (int) tmpb.size();
    assert(tmpA.rows == tmpA.cols);
    assert(tmpA.rows == n);
    Matrix<U> A(tmpA);
    vector<U> b(tmpb);

    upper_triangle_pp(A,b);
    vector<U> x(b.size());
    x = back_substitution(A,b);
    
    return x;
}


template <class U>
void upper_triangle_pp(Matrix<U> & A, vector<U> & b)
{
    // square matrix is cheked by the call function
    int num = (int) b.size();
    U s;
    for (int k = 0; k < num-1; k++)
    {
        int kmax = k;
        for (int i = k + 1; i < num; i++)
        {
            if ( abs(A(kmax,k)) < abs(A(i,k)) )
                kmax = i;
        }
        swap_row(A, kmax, k);
        swap(b[kmax],b[k]);
        for (int i = k + 1; i < num; i++ )
        {
            s = A(i,k) / A(k,k);
            for ( int j = k; j < num; j++)
                A(i,j) -= A(k,j) * s;
            b[i] -= s * b[k];
        }
    }
}

// LU with partal pivot
template <class U>
vector<U> LU_pp_solver(const Matrix<U> & tmpA, const vector<U> & tmpb)
{
    // check A is square
    int n = (int) tmpb.size();
    assert(tmpA.rows == tmpA.cols);
    assert(tmpA.rows == n);
    Matrix<U> A(tmpA);
    vector<U> b(tmpb);
    
    int size = A.rows;
    Matrix<U> L_(size,size), U_(size,size), P_(size,size);
    LU_decomposition_pp(A, P_, L_, U_);
    
    vector<U> c(b.size());
    vector<U> d(b.size());
    vector<U> x(b.size());
    c = P_ * b;
    d = forward_substitution(L_, c);
    x = back_substitution(U_, d);

    return x;
}

/*
Perform LU decomposition on A with partial pivoting.
Implementation does not overwrite A, uses a local copy
*/
template <class U>
void LU_decomposition_pp(Matrix<U> & A, Matrix<U> & P_, Matrix<U> & L_, Matrix<U> & U_)
{
    // square matrix is cheked by the call function
    int m = A.rows;
    //  L is initialised to zero and initialise P to the identity
    Matrix<U> E_ = A.eye(m);
    // call this P_ to emphasise that this is the permutation matrix on the LHS
    // we will return the inverse of P (same as P transpose) to match SciPy behaviour
    P_ = E_;
    
    // j represent the max index in every row in range from diagonal to right
    int j;
    U s;
    
    for ( int k = 0; k != m-1; k++)
    {
        // Find the index of the largest ABSOLUTE value. np.argmax will return
        // the index of the largest element in an array - different method to above
        vector<U> tmp;
        for (int q = k; q != m; q++)
            tmp.push_back(abs(A(q,k)));
        j = argmax(tmp);
        // j here will be the entry counting down from the k-th entry of A,
        //so add on k for use as an index for the matrix A!
        j += k;
        // do the swap or A's rows
        swap_row(A, j, k);
        // and record this swap in the matrix P_
        swap_row(P_, j, k);
        swap_row(L_, j, k);
        
        for (int i = k+1; i != m; i++)
        {
            s = A(i,k) / A(k,k);
            for (int q = k; q != m; q++)
                A(i,q) -= A(k,q) * s;
            
            L_(i,k) = s;
        }
    }
    // the upper-triangular matrix will now be stored in A
    U_ = A;
    // add in the diagonal of ones into L
    L_ = L_ + E_;
}

// LU method without partial pivot method
template <class U>
vector<U> LU_solver(const Matrix<U> & tmpA, const vector<U> & tmpb)
{
    // check A is square
    int n = (int) tmpb.size();
    assert(tmpA.rows == tmpA.cols);
    assert(tmpA.rows == n);
    Matrix<U> A(tmpA);
    vector<U> b(tmpb);
    
    Matrix<U> L(A.rows,A.cols);
    LU_decomposition(A,L);
    
    vector<U> d(b.size());
    vector<U> x(b.size());
    d = forward_substitution(L, b);
    x = back_substitution(A, d);

    return x;
}

template <class U>
void LU_decomposition(Matrix<U> & A, Matrix<U> & L)
{
    // square matrix is cheked by the call function
    int n = A.rows;
    U s;
    // Loop over each pivot row - don't need to consider the final row as a pivot
    for (int k = 0; k != n-1; k++)
    {
        // Loop over each equation below the pivot row - now we do need to consider the last row
        for (int i = k+1; i != n; i++)
        {
            // Define the scaling factor outside the innermost
            // loop otherwise its value gets changed.
            s = A(i,k) / A(k,k);
            for (int j = k; j != n; j++)
                A(i,j) -= s * A(k,j);
            // store the scaling factors which make up the lower triangular matrix
            L(i,k) = s;
        }
    }
    // add in the ones on the main diagonal to L
    L = L + L.eye(n);
    // A now is the upper triangular matrix U
}


// Jacobi method

// Iterate up to  user-specified max number of iterations, we will 'break'
// this loop if we hit our convergence tolerance
template <class U>
vector<U> Jacobi(const Matrix<U> & tmpA, const vector<U> & tmpb, int iter, int pre)
{
    int n = (int) tmpb.size();
    assert(tmpA.rows == tmpA.cols);
    assert(tmpA.rows == n);
    Matrix<U> A(tmpA);
    vector<U> b(tmpb);
    
    int rows = A.rows;
    vector<U> x(rows);
    vector<U> x_new(rows);
    long double tol = pow(10,-pre);

//    cout << "tolerance is:" << tol << endl;
    long double residual;
    int count = 0;
    
    for ( int k = 0; k != iter; k++)
    {
        for ( int i = 0; i != rows; i++)
            x_new[i] = (1.0 / A(i,i)) * ( b[i] - auxiliar(A, x, i) );
        residual = norm( (A * x_new) - b);
        // if less than our required tolerance jump out of the iteration and end.
        if ( residual < tol)
            break;
        count++;
        x = x_new;
    }

    return x;
}

// function that is used for compute dot product of row of A with x except the element i.
template <class U>
long double auxiliar(Matrix<U> & A, const vector<U> & x, int i)
{
    int cols = A.cols;
    long double result = 0;
    for (int j = 0; j != cols; j++)
    {
        if ( j == i)
            result += 0;
        else
            result += A(i,j) * x[j];
    }
    return result;
}

// fuction that compute the norm two of a vector
template <class T>
long double norm(const vector<T> & x)
{
    long double result = 0;
    for (int i = 0; i < x.size(); i++)
        result += x[i] * x[i];

    return sqrt(result);
}


// Gauss Seidel
template <class U>
U sumMul(const Matrix<U> A, const vector<U> b, vector<U> x0, vector<U> tmpX, int i) {
    U s = 0;
    int c = A.cols;
    int r = A.rows;
    if(i > 0)
        for(int x = 0; x < i; x++)
            s += A.values[i*c+x] * tmpX[x];
    
    for (int x = i + 1; x < r; x++) {
        s += A.values[i*c+x] * x0[x];
    }
    return s;
}

template <class U>
vector<U> gauss_seidel(const Matrix<U> & tmpA, const vector<U> & tmpb, vector<U> x0)
{
    int n = (int) tmpb.size();
    assert(tmpA.rows == tmpA.cols);
    assert(tmpA.rows == n);
    Matrix<U> A(tmpA);
    vector<U> b(tmpb);

    U nearZero = 1e-6;
    U err = 0;
    U tmp = 0;
    vector<U> x(n);
    vector<U> tmpX(n);
    for(int i = 0; i < n; i++)
        tmpX[i] = 1;
    do{
        for(int i = 0; i < A.cols; i++)
        {
            tmp = (b[i] - sumMul(A, b, x0, tmpX, i)) / max(A.values[i*A.cols+i], nearZero);
            tmpX[i] = tmp;
        }
        err = 0;
        for(int i = 0; i < A.cols; i++)
            err += abs(x0[i] - tmpX[i]);
        for(int i = 0; i < A.cols; i++)
            x0[i] = tmpX[i];
    } while (err > 1e-6);
    
    for(int i = 0; i < A.cols; i++)
        x[i] = tmpX[i];

    return x;
}

// Conjugate Gradient
template <class U>
U innerProduct(const vector<U> &u, const vector<U> &v){
    // Inner product of U and V
    if(u.size() != v.size()) return 0;
    U sum = 0;
    for(int i = 0; i < u.size(); i++)
        sum += u[i] * v[i];
    return sum;
}

template <class U>
vector<U> Conjugate_Gradient(const Matrix<U> & tmpA, const vector<U> & tmpb, long double tol=1.0e-6)
{
    int n = (int) tmpb.size();
    assert(tmpA.rows == tmpA.cols);
    assert(tmpA.rows == n);
    Matrix<U> A(tmpA);
    vector<U> b(tmpb);
    // interpretation of "zero"
    const U nearZero = 1.0e-5;

    vector<U> x(n);
    for(int i = 0; i < n; i++)
        x[i] = 0.1;
    vector<U> r(n);
    vector<U> tmp(n);
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++)
            tmp[i] += A.values[i*A.cols+j] * x[j];
        r[i] = (b[i] - tmp[i]);
    }
    vector<U> p = r;
    U rho = innerProduct(r, r);
    int k = 0;
    
    while(k < n){
        // Store previous residual
        vector<U> ap(n);
        for(int i = 0; i < n; i++)
            for(int j = 0; j < n; j++)
                ap[i] += A.values[i*A.cols+j] * p[j];
        U alpha = rho / max(innerProduct(p, ap), nearZero);
        // Next estimate of solution
        for(int i = 0; i < p.size(); i++)
            x[i] += alpha * p[i];
        // x = x + alpha * p;
        // Residual
        for(int i = 0; i < ap.size(); i++)
            r[i] -= alpha * ap[i];
        // r = r - alpha * ap;
        // Convergence test
        U rsnew = innerProduct(r, r);
        if(sqrt(rsnew) < tol) break;
        
        U beta = rsnew / max(rho, nearZero);
        // Next gradient
        for(int i = 0; i < p.size(); i++)
            p[i] = r[i] + beta * p[i];
        //p = r + beta * p;
        rho = rsnew;
        k++;
   }
   return x;
}

// ------------------------multiple b method------------------------

// function that is same as back_substitution but with function that can deal with Matrix form b.
template <class U>
Matrix<U> back_substitution_mb(Matrix<U> & A, Matrix<U> & b)
{
    int n = A.rows;
    vector<U> s(b.cols);
    Matrix<U> x(A.cols, b.cols);
    for (int k = n-1; k != -1; k--)
    {
        for (int i = 0; i != s.size(); i++)
            s[i] = 0;
        for ( int q = 0; q != b.cols; q++)
        {
            for (int j = k+1; j != n; j++)
                s[q] += A(k,j) * x(j,q);
            x(k,q) = (b(k,q) - s[q]) / A(k,k);
        }
    }
    
    return x;
}

// function that is same as forward_substitution but with function that can deal with Matrix form b.
template <class U>
Matrix<U> forward_substitution_mb(Matrix<U> & A, Matrix<U> & b)
{
    int n = A.rows;
    vector<U> s(b.cols);
    Matrix<U> x(A.cols, b.cols);
    for (int k = 0; k != n; k++)
    {
        for (int i = 0; i != s.size(); i++)
            s[i] = 0;
        for ( int q = 0; q != b.cols; q++)
        {
            for (int j = 0; j != k; j++)
                s[q] += A(k,j) * x(j,q);
            x(k,q) = (b(k,q) - s[q]) / A(k,k);
        }
    }
    
    return x;
}

// Gauss Elimination method with multiple b

// polymorphism which is realised by overloading function GE to deal with Matrix form b
template <class U>
Matrix<U> GE(const Matrix<U> & tmpA, const  Matrix<U> & tmpb)
{
    int n = (int) tmpb.rows;
    assert(tmpA.rows == tmpA.cols);
    assert(tmpA.rows == n);
    Matrix<U> A(tmpA);
    Matrix<U> b(tmpb);
    
    upper_triangle_mb(A, b);
    Matrix<U> x(A.cols, b.cols);
    x = back_substitution_mb(A, b);

    return x;
}
// function that is same as upper_triangle but with function that can deal with Matrix form b.
template <class U>
void upper_triangle_mb(Matrix<U> & A, Matrix<U> & b)
{
    int n = A.rows;
    U s;
    for (int k = 0; k != n-1; k++)
        for ( int i = k+1; i != n; i++)
        {
            s = A(i,k) / A(k,k);
            for ( int j = k; j != n; j++)
                A(i,j) -= s * A(k,j);
            for ( int j = 0; j != b.cols; j++ )
                b(i,j) -= s * b(k,j);
        }
}

// Gauss_Jordan method

// polymorphism which is realised by overloading function Gauss_Jordan to deal with Matrix form b
template <class U>
Matrix<U> Gauss_Jordan(const Matrix<U> & tmpA, const Matrix<U> & tmpb)
{
    int n = (int) tmpb.rows;
    assert(tmpA.rows == tmpA.cols);
    assert(tmpA.rows == n);
    Matrix<U> A(tmpA);
    Matrix<U> b(tmpb);

    upper_triangle_pp_finite_precision_mb(A,b);
    Matrix<U> x(A.cols, b.cols);
    x = back_substitution_mb(A,b);
    
    return x;
}

// function that is same as forward_substitution_pp but with function that can deal with Matrix form b.
template <class U>
void upper_triangle_pp_finite_precision_mb(Matrix<U> & A, Matrix<U> & b)
{
    int num = (int) b.rows;
    U s;
    for (int k = 0; k < num-1; k++)
    {
        int kmax = k;
        for (int i = k + 1; i < num; i++)
        {
            if ( abs(A(kmax,k)) < abs(A(i,k)) )
                kmax = i;
        }
        swap_row(A, kmax, k);
        swap_row(b, kmax, k);
        for (int i = k + 1; i < num; i++ )
        {
            s = A(i,k) / A(k,k);
            for ( int j = k; j < num; j++)
                A(i,j) -= A(k,j) * s;
            for (int j = 0; j != b.cols; j++)
                b(i,j) -= s * b(k,j);
        }
    }
}

// LU method without partial pivot method with multiple b

// polymorphism which is realised by overloading function LU_solver to deal with Matrix form b
template <class U>
Matrix<U> LU_solver(const Matrix<U> & tmpA, const Matrix<U> & tmpb)
{
    int n = (int) tmpb.rows;
    assert(tmpA.rows == tmpA.cols);
    assert(tmpA.rows == n);
    Matrix<U> A(tmpA);
    Matrix<U> b(tmpb);
    
    Matrix<U> L(A.rows,A.cols);
    LU_decomposition(A,L);
    
    Matrix<U> d(A.cols,b.cols);
    Matrix<U> x(A.cols,b.cols);
    d = forward_substitution_mb(L, b);
    x = back_substitution_mb(A, d);

    return x;
}

template <class U>
Matrix<U> Cholesky_Decomposition(Matrix<U> A){
    int n = A.rows;
    Matrix<U> lower(n,n);
    Matrix<U> L(n,n), LT(n,n);
  
    // Decomposing a matrix into Lower Triangular
    for(int i = 0; i < n; i++) {
        for(int j = 0; j <= i; j++) {
            int sum = 0;
            if (j == i) // summation for diagnols
            {
                for(int k = 0; k < j; k++)
                    sum += pow(lower.values[j*lower.cols+k], 2);
                lower.values[j*lower.cols+j] = sqrt(A.values[j*A.cols+j] -
                                        sum);
            } else {
  
                // Evaluating L(i, j) using L(j, j)
                for (int k = 0; k < j; k++)
                    sum += (lower.values[i*lower.cols+k] * lower.values[j*lower.cols+k]);
                lower.values[i*lower.cols+j] = (A.values[i*A.cols+j] - sum) / lower.values[j*lower.cols+j];
            }
        }
    }
    for (int i = 0; i < n; i++) {
        // Lower Triangular
        for (int j = 0; j < n; j++){
            L.values[i*L.cols+j] = lower.values[i*lower.cols+j];
        }
    }
    return L;
}

template <class U>
vector<U> cholesky(Matrix<U> A, vector<U> b){
    int n = A.rows;
    vector<U> x(n);
    vector<U> y(n);
    Matrix<U> L(n,n), LT(n,n);
    L = Cholesky_Decomposition(A);
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            LT.values[i*LT.cols+j] = L.values[j*L.cols+i];
    y = forward_substitution(L,b);
    x = back_substitution(LT,y);
    return x;
}

/*
    GMRES
*/

// Function that returns an mxn matrix with entries that
// are the same as matrix A, where possible
template <class U>
Matrix<U> resize(Matrix<U> A, int m, int n){
    int p,q;
    Matrix<U> Mout(m,n);
    
    // select lowest of each matrix dimension
    if(m <= A.rows){
        p = m;
    }else{
        p = A.rows;
    }
    if(n <= A.cols){
        q = n;
    }else{
        q = A.cols;
    }
// copy across relevant values
    for(int i = 0; i < p; i++){
        for(int j = 0; j < q; j++){
            Mout.values[i*n+j] = A.values[i*A.cols+j];
        }
    }
    return Mout;
}

template <class U>
vector<U> GMRES(Matrix<U> A, vector<U> b, vector<U> x0, double tol=1e-10){
    // double tol=1e−6;
    // determine initial residual, r0 in vector form
    
    vector<U> r0(A.rows);
    double r0_tmp = 0;
    for(int i = 0; i < A.rows; i++){
        r0_tmp = 0;
        for(int j = 0; j < A.cols; j++)
            r0_tmp += A(i,j) * x0[j];
        r0[i] = b[i] - r0_tmp;
    }
    // need this in least square part later
    double normr0 = norm(r0);
    // initialise to enter while loop
    double residual = 1.0;
    // intialise vector v
    vector<U> v(r0.size());
    for(int i = 0; i < r0.size(); i++)
        v[i] = r0[i]/normr0;
    
    // Arnoldi/GMRES step index
    int k = 1;
    // Declare Givens rotation matrix, initialise Jtotal;
    Matrix<U> J(2,2), Jtotal(2,2);

    Matrix<U> temp_eye(2,2);
    
    // Set the entries of B to be the same as those in A
    for(int i = 0; i < 2; i++){
        temp_eye.values[i*temp_eye.cols+i] = 1;
    }
    Jtotal = temp_eye;
    // intialise H, declare tempMat, V, w
    Matrix<U> H(1,1), Htemp(1,1), HH(1,1), bb(1,1), c(1,1), cc(1,1);
    Matrix<U> V(A.rows, 1), Vold(A.rows,1), hNewCol(1,1);
    vector<U> w(A.rows), vj(v.size());
    bb.values[0]=normr0;
    
    // initialise matrix V (matrix of orthogonal basis vectors)
    for(int x = 0; x < A.rows; ++x)
        V.values[x] = v[x];
    while(residual > tol){
        // update Vold (used for checking Arnoldi later)
        Vold = V;
        H = resize(H,k+1,k);
        std::cout<<H<<std::endl;
        // Arnoldi steps (using Gram−Schmidt process)
        for(int x = 0; x < A.rows; x++){
            double w_tmp = 0;
            for(int y = 0; y < A.cols; y++)
                w_tmp += A.values[x*A.cols+y] * v[y];
            w[x] = w_tmp;
        }
        for(int j = 0; j < k; j++){
            for(int i = 0; i < V.rows; i++){
                // set the vector vj to be jth column of V
                vj[i] = V.values[i*V.cols+j];
            }
            Matrix<U> tempMat(1, 1);
            for(int x = 0; x < vj.size(); x++)
                tempMat.values[0] += vj[x] * w[x];
            // these two lines calculate the inner product
            H.values[j*H.cols+k-1] = tempMat.values[0];
            vector<U> vj_tmp(vj.size());
            for(int x = 0; x < vj.size(); x++)
                vj_tmp[x] = H.values[j*H.cols+k-1] * vj[x];
            for(int x = 0; x < w.size(); x++)
                w[x] = w[x] - vj_tmp[x];
        }
        H.values[k*H.cols+k-1] = norm(w);
        for(int x = 0; x < w.size(); x++){
            v[x] = w[x] / H.values[k*H.cols+k-1];
        }
        // add one more column to matrix V
        V = resize(V,V.rows,k+1);
        for(int i = 0; i < v.size(); i++){
            // copy entries of v to new column of V
            //V(i,k)=v[i];
            V.values[i*V.cols+k] = v[i];
        }
        
//        std::cout<< "(k = " << k << ") :latest matrix, V: \n\n" << V;
//        std::cout << "(k = " << k <<") :latest matrix, H: \n\n" << H;
//        std::cout << "check: AV[k] = V[k+1]H: \n\n" << A*Vold << V*H;
        /////////////////////////////// Least squares step //////////////////////
        if (k == 1){
            // First pass through, Htemp=H
            Htemp = H;
        }else{
            // for subsequent passes, Htemp=Jtotal*H
            Jtotal = resize(Jtotal,k+1,k+1);
            Jtotal.values[k*Jtotal.cols+k]=1;
            Htemp = Jtotal*H;
        }

        Matrix<U> t(k-1,k-1);
        // Set the entries of B to be the same as those in A
        std::cout<<"123"<<std::endl;
        for(int i = 0; i < k-1; i++){
            t.values[i*t.cols+i] = 1;
        }
        J = t;

        J = resize(J,k+1,k+1);
        J.values[(k-1)*J.cols+k-1] = Htemp.values[(k-1)*Htemp.cols+k-1] / pow(pow(Htemp.values[(k-1)*Htemp.cols+k-1], 2)+ pow(Htemp.values[k*Htemp.cols+k-1], 2), 0.5);

        J.values[(k-1)*J.cols+k] = Htemp.values[k*Htemp.cols+k-1] / pow(pow(Htemp.values[(k-1)*Htemp.cols+k-1], 2)+ pow(Htemp.values[k*Htemp.cols+k-1], 2), 0.5);

        J.values[k*J.cols+k-1] = -Htemp.values[k*Htemp.cols+k-1] / pow(pow(Htemp.values[(k-1)*Htemp.cols+k-1], 2)+ pow(Htemp.values[k*Htemp.cols+k-1], 2), 0.5);

        J.values[k*J.cols+k] = Htemp.values[(k-1)*Htemp.cols+k-1] / pow(pow(Htemp.values[(k-1)*Htemp.cols+k-1], 2)+ pow(Htemp.values[k*Htemp.cols+k-1], 2), 0.5);

        // combine together with previous Givens rotations
        Jtotal=J*Jtotal;
        HH = Jtotal*H;
        U temp = 0;
        
        for (int a = 0; a < J.rows; a++){
            // cols (q)
            for (int b = 0; b < Jtotal.cols; b++){
                // dot product step (n sums)
                for (int c = 0; c < J.cols; c++)
                    temp += J.values[a * J.cols + c] * Jtotal.values[b + c*Jtotal.cols];
                // Set the C Matrix values
                HH.values[a * Jtotal.cols + b] = temp;
                // reset temp
                temp = 0;
            }
        }

        // rows (m)
        for (int a = 0; a < Jtotal.rows; a++){
            // cols (q)
            for (int b = 0; b < H.cols; b++){
                // dot product step (n sums)
                for (int c = 0; c < Jtotal.cols; c++)
                    temp += Jtotal.values[a * Jtotal.cols + c] * H.values[b + c*H.cols];
                // Set the C Matrix values
                HH.values[a * H.cols + b] = temp;
                // reset temp
                temp = 0;
            }
        }
        
        
        for(int i = 0; i < k+1; i++){
            for(int j = 0; j < k; j++){
                // set all 'small' values to zero
                if(fabs(HH.values[i*HH.cols+j]) < 1e-15){
                    HH.values[i*HH.cols+j] = 0;
                }
            }
        }
        
        bb = resize(bb,k+1,1);
        c = Jtotal*bb;
        // rows (m)
        for (int x = 0; x < Jtotal.rows; x++){
            // cols (q)
            for (int y = 0; y < H.cols; y++){
                // dot product step (n sums)
                for (int z = 0; z < Jtotal.cols; z++)
                    temp += Jtotal.values[x * Jtotal.cols + z] * bb.values[y + z*bb.cols];
                // Set the C Matrix values
                c.values[x * bb.cols + y] = temp;
                // reset temp
                temp = 0;
            }
        }
        residual = fabs(c(k,0));
        std::cout<< "residual: \n\n" << residual;
        k++;
    }
    // Extract upper triangular square matrix
    HH = resize(HH,HH.rows-1,HH.cols);
    cc = resize(c,HH.rows,1);
    vector<U> tmp_cc(cc.rows);
    for(int x = 0; x < cc.rows; x++)
        tmp_cc[x] = cc.values[x];
    
    // solve linear system
    vector<U> y = GE(HH, tmp_cc);
    // chop the newest column off of matrix V
    V = resize(V,V.rows,V.cols-1);
    U q = 0;
    vector<U> x(A.rows);
    for(int a = 0; a < V.rows; a++){
        q = 0;
        for(int b = 0; b < y.size(); b++)
            q += V.values[a*V.cols+b] * y[b];
        x[a] = x0[a] + q;
    }
    return x;
}
