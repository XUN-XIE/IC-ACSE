#ifndef Solver_h
#define Solver_h

#include "Matrix.h"

template <class T>
class solver : private Matrix<T>
{
public:
    // solver object can only be constructed by input a Matrix object
    solver(const Matrix<T> & A) : Matrix<T>(A) {}
    
    // produce random value b for given Matrix
    template <class U>
    friend vector<U> produce_b(const Matrix<U> & A, int min, int max);
    
    // produce random value b for given Matrix
    template <class U>
    friend vector<U> rand_produce_b(const Matrix<U> & A, int min, int max);

    // swap given index rows in a given matrix
    template <class U>
    friend void swap_row(Matrix<U> & mat, int i, int j);
    
    // ------------------------Linear Solver Algorithms------------------------
    
    // ensure the matrix will not change in the interface function:GE, Gauss_Jordan, LU_solver.
    
    // -------------Gauss Family method----------------
    
    // back and forward subsititution algorithms used for Gauss Elimination, Gauss Jordan, LU decomposition.
    template <class U>
    friend vector<U> back_substitution(Matrix<U> & A, vector<U> & b);
    template <class U>
    friend vector<U> forward_substitution(Matrix<U> & A, vector<U> & b);
    
    // Gauss Elimination method
    template <class U>
    friend vector<U> GE(const Matrix<U> & tmpA, const  vector<U> & tmpb);
    template <class U>
    friend void upper_triangle(Matrix<U> & A, vector<U> & b);
    
    // Gauss_Jordan method
    template <class U>
    friend vector<U> Gauss_Jordan(const Matrix<U> & tmpA, const vector<U> & tmpb);
    template <class U>
    friend void upper_triangle_pp_finite_precision(Matrix<U> & A, vector<U> & b);
    template <class U>
    friend vector<U> back_substitution_finite_precision(Matrix<U> & A, vector<U> & b);
    
    // Gauss Seidel
    template <class U>
    friend double sumMul(const Matrix<U> A, const vector<U> b, vector<U> x0, vector<U> tmpX, int i);
    template <class U>
    friend vector<U> gauss_seidel(const Matrix<U> & tmpA, const vector<U> & tmpb, vector<U> x0);

    // -------------LU Family method----------------
    
    // LU method with partial pivot method
    template <class U>
    friend vector<U> LU_pp_solver(const Matrix<U> & tmpA, const vector<U> & tmpb);
    template <class U>
    friend void LU_decomposition_pp(Matrix<U> & A, Matrix<U> & P_, Matrix<U> & L_, Matrix<U> & U_);
    
    // LU method without partial pivot method
    template <class U>
    friend vector<U> LU_solver(const Matrix<U> & tmpA, const vector<U> & tmpb);
    template <class U>
    friend void LU_decomposition(Matrix<U> & A, Matrix<U> & L);
    
    // -------------iteration method----------------
    
    // Jacobi's method
    template <class U>
    friend vector<U> Jacobi(const Matrix<U> & tmpA, vector<U> & b, int iter, int pre);
    template <class U>
    friend U auxiliar(Matrix<U> & mat, const vector<U> & x, int i);
    
    // Conjugate Gradient
    template <class U>
    friend U innerProduct(const vector<U> &u, const vector<U> &v);
    template <class U>
    friend vector<U> Conjugate_Gradient(const Matrix<U> & tmpA, const Matrix<U> & tmpb, double tol);
    
    // ------------------------multiple b method------------------------
    
    template <class U>
    friend Matrix<U> back_substitution_mb(Matrix<U> & A, Matrix<U> & b);
    template <class U>
    friend Matrix<U> forward_substitution_mb(Matrix<U> & A, Matrix<U> & b);
    
    // Gauss Elimination method with multiple b
    template <class U>
    friend Matrix<U> GE(const Matrix<U> & tmpA, const  Matrix<U> & tmpb);
    template <class U>
    friend void upper_triangle_mb(Matrix<U> & A, Matrix<U> & b);
    
    // Gauss_Jordan method
    template <class U>
    friend Matrix<U> Gauss_Jordan(const Matrix<U> & tmpA, const Matrix<U> & tmpb);
    template <class U>
    friend void upper_triangle_pp_finite_precision_mb(Matrix<U> & A, Matrix<U> & b);
    template <class U>
    friend Matrix<U> back_substitution_finite_precision_mb(Matrix<U> & A, Matrix<U> & b);

    // LU method without partial pivot method with multiple b
    template <class U>
    friend Matrix<U> LU_solver(const Matrix<U> & tmpA, const Matrix<U> & tmpb);
    
    //Choleskey
    template <class U>
    friend Matrix<U> Cholesky_Decomposition(Matrix<U> A);
    template <class U>
    friend vector<U> cholesky(Matrix<U> A, vector<U> b);
    
    // GMRES
    template <class U>
    friend Matrix<U> resize(Matrix<U> A, int m, int n);
    template <class U>
    friend vector<U> GMRES(Matrix<U> A, Matrix<U> b, Matrix<U> x0, double tol);
};

template <class T>
long double norm(const vector<T> & x);

#endif /* Solver_h */
