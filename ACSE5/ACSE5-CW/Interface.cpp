#include <iostream>
#include <ctime>
#include <fstream>
#include "Matrix.cpp"
#include "Solver.cpp"
#include "CSRMatrix.cpp"
#include "Interface.h"

using namespace std;

double test_data_1[100] = { 76,   2, -31,   5,  -4,  -4,  73,   9, -15, -25,   2,  84,  -6,
 36,  23,  10,  14,  33, -26, -34, -31,  -6,  52,  -5,  -2,   8,
-38,  -7,  -9,  12,   5,  36,  -5,  83,  29, -12, -10,  51,  23,
  3,  -4,  23,  -2,  29,  56,  18, -13,  36,  25,  11,  -4,  10,
  8, -12,  18,  48, -12,  21, -27,   5,  73,  14, -38, -10, -13,
-12,  94, -12, -33, -34,   9,  33,  -7,  51,  36,  21, -12,  64,
 23,   6, -15, -26,  -9,  23,  25, -27, -33,  23, 106,  26, -25,
-34,  12,   3,  11,   5, -34,   6,  26,  45 };

double test_data_2[255] = { 115, 9, 22, -13, -51, -35, 44, -7, -1, -24, 12,
           19, 2, -59, 15, 9, 118, -15, 53, 50, 17, 7,
           55, 14, 11, 44, -15, -94, 16, -26, 22, -15, 94,
           -2, -37, 42, -40, -10, 5, 4, -28, 9, -2, 15,
           -7, -13, 53, -2, 162, -24, 8, 17, 98, -15, -2,
           -44, -65, -22, -4, 5, -51, 50, -37, -24, 172, 53,
           31, 17, -37, 40, 115, 44, -51, 48, -4, -35, 17,
           42, 8, 53, 186, -36, -22, 23, 62, -4, 15, 10,
           22, 3, 44, 7, -40, 17, 31, -36, 125, -10, -70,
           -2, 52, -8, -11, -5, 68, -7, 55, -10, 98, 17,
           -22, -10, 118, 16, 3, 32, -29, -51, 6, -28, -1,
           14, 5, -15, -37, 23, -70, 16, 129, 5, -35, -36,
           3, -2, -6, -24, 11, 4, -2, 40, 62, -2, 3,
           5, 121, 7, 26, -14, 29, 2, 12, 44, -28, -44,
           115, -4, 52, 32, -35, 7, 195, 37, -107, 63, -1,
           19, -15, 9, -65, 44, 15, -8, -29, -36, 26, 37,
           115, 6, 23, -16, 2, -94, -2, -22, -51, 10, -11,
           -51, 3, -14, -107, 6, 156, -94, -3, -59, 16, 15,
           -4, 48, 22, -5, 6, -2, 29, 63, 23, -94, 174,
           48, 15, -26, -7, 5, -4, 3, 68, -28, -6, 2,
           -1, -16, -3, 48, 117 };

double test_data_3[400] = { 137, -16, -20, -21, 30, 62, 27, 20, 24, 0, 0, 3, 39,
        2, 30, -3, 110, 47, 38, 13, -16, 190, -31, 9, 2, -39,
        38, 10, -4, 13, 2, 38, -25, 25, 52, 72, -76, -26, 64,
        5, -20, -31, 154, -34, 23, 43, -8, 7, 17, 35, 52, -40,
        -5, 76, 21, 5, -53, 18, -31, -16, -21, 9, -34, 133, -37,
        -28, -24, 0, -36, -37, -4, 8, -52, -35, -37, 7, -39, -8,
        -3, -99, 30, 2, 23, -37, 149, 38, -39, -9, 21, -25, 29,
        0, 3, 63, 7, -14, -32, 21, -7, 15, 62, -39, 43, -28,
        38, 202, -19, 37, 26, 25, -21, -25, -11, 25, 38, 27, 32,
        7, -8, 11, 27, 38, -8, -24, -39, -19, 122, -32, 28, -17,
        0, -8, 29, 5, -17, 55, 10, 20, 20, 41, 20, 10, 7,
        0, -9, 37, -32, 209, -38, 64, -37, 72, -8, 12, 73, 2,
        24, -76, -11, -3, 24, -4, 17, -36, 21, 26, 28, -38, 109,
        12, 42, -53, 96, 37, 32, -6, 7, 24, 33, 20, 0, 13,
        35, -37, -25, 25, -17, 64, 12, 203, 32, 18, 58, 25, 112,
        -22, 53, -43, 36, -2, 0, 2, 52, -4, 29, -21, 0, -37,
        42, 32, 104, 1, 52, 35, 4, 10, -6, 22, 22, 26, 3,
        38, -40, 8, 0, -25, -8, 72, -53, 18, 1, 168, -41, -2,
        30, 30, 31, -11, -52, 25, 39, -25, -5, -52, 3, -11, 29,
        -8, 96, 58, 52, -41, 213, 23, 32, -62, 110, -9, 68, 51,
        2, 25, 76, -35, 63, 25, 5, 12, 37, 25, 35, -2, 23,
        145, 23, 24, -34, -43, 0, -10, 30, 52, 21, -37, 7, 38,
        -17, 73, 32, 112, 4, 30, 32, 23, 207, -41, 53, -9, 20,
        -16, -3, 72, 5, 7, -14, 27, 55, 2, -6, -22, 10, 30,
        -62, 24, -41, 131, -90, -7, 5, 48, 110, -76, -53, -39, -32,
        32, 10, 24, 7, 53, -6, 31, 110, -34, 53, -90, 254, 7,
        35, 28, 47, -26, 18, -8, 21, 7, 20, -76, 24, -43, 22,
        -11, -9, -43, -9, -7, 7, 137, -20, 24, 38, 64, -31, -3,
        -7, -8, 20, -11, 33, 36, 22, -52, 68, 0, 20, 5, 35,
        -20, 120, 14, 13, 5, -16, -99, 15, 11, 41, -3, 20, -2,
        26, 25, 51, -10, -16, 48, 28, 24, 14, 182 };

double qqq[9] = {4,12,-16,12,37,-43,-16,-43,98};

Matrix<double> A_1(10,10, test_data_1);
Matrix<double> A_2(15,15, test_data_2);
Matrix<double> A_3(20,20, test_data_3);
vector<double> b1 = produce_b(A_1, -100, 100);
vector<double> b2 = produce_b(A_2, -100, 100);
vector<double> b3 = produce_b(A_3, -100, 100);
vector<double> x0_1 = produce_b(A_1, -100, 100);
vector<double> x0_2 = produce_b(A_2, -100, 100);
vector<double> x0_3 = produce_b(A_3, -100, 100);

Matrix<double> test_A(3,3, qqq);
vector<double> bbb = produce_b(test_A, -100, 100);
vector<double> xxx = produce_b(test_A, -100, 100);


template <class T>
void test_GE(Matrix<T> & A, vector<T>& b);

template <class T>
void test_LU(Matrix<T> & A, vector<T>& b);

template <class T>
void test_LU_pp(Matrix<T> & A, vector<T>& b);

template <class T>
void test_Jacobi(Matrix<T> & A, int iter, int pre, vector<T>& b);

template <class T>
void test_gauss_seidel(const Matrix<T> A, const vector<T> b, vector<T> x0);

template <class T>
void test_CG(const Matrix<T> &A, const vector<T> &b, double tol);


void test_all_given_data();

int * produce_random_matrix(int n, int min, int max);
void test_random_matrix(int size, int min, int max);

double test_diefferent_size(void (*function)(Matrix<double> & A));

void time_complexity();

void test_mb();

Interface::Interface()
{
    interfaceIntro();
}

Interface::~Interface()
{
}

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
    char select;
    cin >> select;
    
    switch (select)
    {
        case 'y': break;
        case 'n': exit(0);
        default: interfaceIntro();
    }

    interfaceSelectMatrix();
}

void Interface::interfaceSelectMatrix()
{
    cin.clear();
    cout << endl;
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
    string select;
    
    cin >> select;
    
    // string to char
    char* select_char = new char[1];
    if (select.size() != 1) { *select_char = '#'; }
    else { copy(select.begin(), select.end(), select_char); }
    
    switch (*select_char)
    {
        case '1': interfaceDenseMatrix(); break;
        case '2': interfaceInvalid("To be continue, not yet to finish CSR matrix.\n This is our C++ Punsiher future work"); break;
        case 'b': system("CLS"); cin.ignore(); interfaceIntro(); break;
        case 'x': exit(0);
        default: interfaceInvalid(""); interfaceSelectMatrix();
    }
    delete[] select_char;
}

void Interface::interfaceInvalid(string message)
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << " ----------------------------------" << endl;
    cout << "|       Invalid Selection          |" << endl;
    cout << " ----------------------------------" << endl;
    cout << message << endl;
    cout << endl;
    cout << " ----------------------------------" << endl;
    cout << "| Press enter to return...         |" << endl;
    cout << " ----------------------------------" << endl;
    cin.get();
}

int Interface::loadMatrixData()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.clear();
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
    string select;
    
    cin >> select;
    
    int data = 0;
    // string to char
    char* select_char = new char[1];
    if (select.size() != 1) { *select_char = '#'; }
    else { copy(select.begin(), select.end(), select_char); }
    
    switch (*select_char){
        case '1': data = 1; break;
        case '2': data = 2; break;
        case '3': data = 3; break;
        case 'b': system("CLS"); cin.ignore(); interfaceSelectMatrix(); break;
        case 'x': exit(0);
        default: interfaceInvalid(""); loadMatrixData();
    }
    
    delete[] select_char;
    return data;
}

void Interface::change_b_Matrix(int choice)
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.clear();
    cout << " ------------------------------------------" << endl;
    cout << "|    Do you want to change random matrix?  |:" << endl;
    cout << " ------------------------------------------" << endl;
    cout << endl;
    cout << " --------------------------------" << endl;
    cout << "| a: Yes                         |" << endl;
    cout << "| b: No                          |" << endl;
    cout << " --------------------------------" << endl;
    cout << " >> ";
    string select;
    
    cin >> select;
    
    // string to char
    char* select_char = new char[1];
    if (select.size() != 1) { *select_char = '#'; }
    else { copy(select.begin(), select.end(), select_char); }
    
    if(*select_char == 'a'){
        if(choice == 1)     b1 = rand_produce_b(A_1, -100, 100);
        else if(choice == 2)     b2 = rand_produce_b(A_2, -100, 100);
        else if(choice == 3)     b3 = rand_produce_b(A_3, -100, 100);
    }else if(*select_char == 'b'){
        return;
    }
    else{
        interfaceInvalid("");
        change_b_Matrix(choice);
    }
    
    delete[] select_char;
}

void Interface::interfaceDenseMatrix()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.clear();
    
    cout << endl;
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
    string select;
    
    cin >> select;
    
    // string to char
    char* select_char = new char[1];
    if (select.size() != 1) { *select_char = '#'; }
    else { copy(select.begin(), select.end(), select_char); }
    
    switch (*select_char){
        case '1': denseAlgorithm('1'); break;
        case '2': denseAlgorithm('2'); break;
        case '3': denseAlgorithm('3'); break;
        case '4': denseAlgorithm('4'); break;
        case '5': denseAlgorithm('5'); break;
        case 'b': system("CLS"); cin.ignore(); interfaceSelectMatrix(); break;
        case 'x': exit(0);
        default: interfaceInvalid(""); interfaceSparseMatrix();
    }
    delete[] select_char;
    
}

void Interface::interfaceSparseMatrix()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.clear();
    cout << endl;
    cout << " ------------------------------------------" << endl;
    cout << "|  Select Algorihm to solve sparse matrix: |:" << endl;
    cout << " ------------------------------------------" << endl;
    cout << endl;
    cout << " -----------------------------------------------" << endl;
    cout << "| 1: Gauss Seidel                               |" << endl;
    cout << "| 2: Conjugate Gradient                         |" << endl;
    cout << "| b: Back                                       |" << endl;
    cout << "| x: Exit                                       |" << endl;
    cout << " -----------------------------------------------" << endl;
    cout << " >> ";
    string select;
    
    cin >> select;
    
    // string to char
    char* select_char = new char[1];
    if (select.size() != 1) { *select_char = '#'; }
    else { copy(select.begin(), select.end(), select_char); }
    
    switch (*select_char){
        case '1': sparseAlgorithm('1'); break;
        case '2': sparseAlgorithm('2'); break;
        case 'b': system("CLS"); cin.ignore(); interfaceSelectMatrix(); break;
        case 'x': exit(0);
        default: interfaceInvalid(""); interfaceDenseMatrix();
    }
    delete[] select_char;
    
}

void Interface::denseAlgorithm(char select_char)
{
    int iter = 1e4;
    int pre = 5;
    int choice = loadMatrixData();
    change_b_Matrix(choice);
    if(choice == 1){
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " Matrix size is 10x10" << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " A Matrix is : \n\n" << A_1 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " b Matrix is : \n\n" << b1 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " x solution is\n" << endl;
        if(select_char == '1')
            test_GE(A_1, b1);
        else if(select_char == '2')
            test_LU(A_1, b1);
        else if(select_char == '3')
            test_gauss_seidel(A_1, b1, x0_1);
        else if(select_char == '4')
            test_LU_pp(A_1, b1);
        else if(select_char == '5')
            test_CG(A_1, b1, 1e-6);
    }else if(choice == 2){
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " Matrix size is 15x15" << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " A Matrix is : \n\n" << A_2 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " b Matrix is : \n\n" << b2 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " x solution is\n" << endl;
        if(select_char == '1')
            test_GE(A_2, b2);
        else if(select_char == '2')
            test_LU(A_2, b2);
        else if(select_char == '3')
            test_gauss_seidel(A_2, b2, x0_2);
        else if(select_char == '4')
            test_LU_pp(A_2, b2);
        else if(select_char == '5')
            test_CG(A_2, b2, 1e-6);
    }else if(choice == 3){
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " Matrix size is 20x20" << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " A Matrix is : \n\n" << A_3 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " b Matrix is : \n\n" << b3 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " x solution is\n" << endl;
        if(select_char == '1')
            test_GE(A_3, b3);
        else if(select_char == '2')
            test_LU(A_3, b3);
        else if(select_char == '3')
            test_gauss_seidel(A_3, b3, x0_3);
        else if(select_char == '4')
            test_LU_pp(A_3, b3);
        else if(select_char == '5')
            test_CG(A_3, b3, 1e-6);
    }
    
    interfaceSelectMatrix();
        
}
void Interface::sparseAlgorithm(char select_char)
{
    int iter = 1e4;
    int pre = 5;
    int choice = loadMatrixData();
    change_b_Matrix(choice);
    
    if(choice == 1){
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " Matrix size is 10x10" << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " A Matrix is : \n\n" << A_1 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " b Matrix is : \n\n" << b1 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " x solution is\n" << endl;
        if(select_char == '1'){
            test_gauss_seidel(A_1, b1, x0_1);
            cout << "test_gauss_seidel 10x10" << endl;
        }else if(select_char == '2'){
            test_CG(A_1, b1, 1e-6);
            cout << "test_gauss_seidel 10x10" << endl;
        }
    }else if(choice == 2){
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " Matrix size is 15x15" << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " A Matrix is : \n\n" << A_2 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " b Matrix is : \n\n" << b2 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " x solution is\n" << endl;
        if(select_char == '1'){
            test_gauss_seidel(A_2, b2, x0_2);
        }else if(select_char == '2'){
            test_CG(A_2, b2, 1e-6);
        }
    }else if(choice == 3){
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " Matrix size is 20x20" << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " A Matrix is : \n\n" << A_3 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " b Matrix is : \n\n" << b3 << endl;
        cout << endl;
        cout << " -----------------------------------------------" << endl;
        cout << " x solution is\n" << endl;
        if(select_char == '1'){
            test_gauss_seidel(A_3, b3, x0_3);
        }else if(select_char == '2'){
            test_CG(A_3, b3, 1e-6);
        }
    }
    
    interfaceSelectMatrix();
}


template <class T>
void test_GE(Matrix<T> & A, vector<T>& b)
{
    cout << "------------\nGE solver:\n";
    vector<T> x;
    x = GE(A,b);
    cout << x << endl;
    cout << "------------    EXIT GAUSS ELIMINATION    --------------" << endl;
}

template <class T>
void test_LU(Matrix<T> & A, vector<T>& b)
{
    cout << "------------\nLU solver:\n";
    vector<T> x;
    x = LU_solver(A,b);
    cout << x << endl;
    cout << "------------       EXIT LU       --------------" << endl;
}

template <class T>
void test_LU_pp(Matrix<T> & A, vector<T>& b)
{
    cout << "------------\nLU pp solver:\n";
    vector<T> x;
    x = LU_pp_solver(A,b);
    cout << x << endl;
    cout << "------------       EXIT LU pp       --------------" << endl;
}


template <class T>
void test_Jacobi(Matrix<T> & A, int iter, int pre, vector<T>& b)
{
    cout << "------------\nJacobi solver:\n";
    vector<T> x;
    x = Jacobi(A,b,iter,pre);
    cout << x << endl;
    cout << "------------     EXIT JACOBI     --------------" << endl;
}

template <class T>
void test_gauss_seidel(Matrix<T> A, vector<T> b, vector<T> x0){
    cout << "------------\nGauss Seidel solver:\n";
    vector<T> x;
    x = gauss_seidel(A, b, x0);
    cout << x << endl;
    cout << "------------     EXIT GAUSS SEIDEL --------------" << endl;
}

template <class T>
void test_CG(const Matrix<T> &A, const vector<T> &b, double tol){
    cout << "------------\nConjugate Gradient solver:\n";
    vector<T> x;
    x = Conjugate_Gradient(A, b, tol);
    cout << x << endl;
    cout << "------------     EXIT CONJUGATE GRADIENT -------" << endl;
}


/*
     without paramater b b by XUN XIE
 */
template <class T>
void test_GE(Matrix<T> & A);

template <class T>
void test_LU(Matrix<T> & A);

template <class T>
void test_LU_pp(Matrix<T> & A);

template <class T>
void test_GJ(Matrix<T> & A);

template <class T>
void test_Jacobi(Matrix<T> & A, int iter, int pre);

template <class T>
void test_GS(Matrix<T> & A);

template <class T>
void test_CG(Matrix<T> & A, double tol);

template <class T>
void test_GE(Matrix<T> & A)
{
    vector<T> b = produce_b(A, -100, 100);
    vector<T> x;
    x = GE(A,b);
    
    cout << "------------\nGE solver:\n";
    cout << "b is : " << b << endl;
    cout << "x is: \n" << x << endl;
}

template <class T>
void test_LU(Matrix<T> & A)
{
    vector<T> b = produce_b(A, -100, 100);
    vector<T> x;    // control random seed
    x = LU_solver(A,b);
    
    cout << "------------\nLU solver:\n";
    cout << "b is : " << b << endl;
    cout << x << endl;
}

template <class T>
void test_LU_pp(Matrix<T> & A)
{
    vector<T> b = produce_b(A, -100, 100);
    vector<T> x;
    x = LU_pp_solver(A,b);
    
    cout << "------------\nLU_pp solver:\n";
    cout << "b is : " << b << endl;
    cout << "x is: \n" << x << endl;
}

template <class T>
void test_GJ(Matrix<T> & A)
{
    vector<T> b = produce_b(A, -100, 100);
    vector<T> x;
    x = Gauss_Jordan(A,b);
    
    cout << "------------\nGauss Jordan solver:\n";
    cout << "b is : " << b << endl;
    cout << x << endl;
}


template <class T>
void test_Jacobi(Matrix<T> & A, int iter, int pre)
{
    vector<T> b = produce_b(A, -100, 100);
    vector<T> x;
    x = Jacobi(A,b,iter,pre);
}

template <class T>
void test_GS(Matrix<T> & A)
{
    vector<T> x0(A.rows, 0.1);
    vector<T> b = produce_b(A, -100, 100);
    vector<T> x(A.rows);
    x = gauss_seidel(A, b, x0);
    
    cout << "------------\nGauss Seidel solver:\n";
    cout << "b is : " << b << endl;
    cout << x << endl;
}

template <class T>
void test_CG(Matrix<T> & A, double tol)
{
    
    vector<T> b = produce_b(A, -100, 100);
    vector<T> x(A.rows);
    x = Conjugate_Gradient(A, b, tol);
    
    cout << "------------\nConjugate Gradient solver:\n";
    cout << "b is : " << b << endl;
    cout << x << endl;
}


void test_all_given_data()
{
        Matrix<long double> A_1(10,10,true);
        for (int i = 0; i != 100; i++)
            A_1.values[i] = test_data_1[i];
        Matrix<long double> A_2(15,15,true);
        for (int i = 0; i != 255; i++)
            A_2.values[i] = test_data_2[i];
        Matrix<long double> A_3(20,20,true);
        for (int i = 0; i != 400; i++)
            A_3.values[i] = test_data_3[i];

        int i = 0;
        clock_t start, end;
        start = clock();
        for (i = 0; i != 1000; i++)
        {
            test_GE(A_1);
            test_GE(A_2);
            test_GE(A_3);
        }
        end = clock();
        cout << "GE time consuming: \n" << end - start << endl;

        start = clock();
        for (i = 0; i != 1000; i++)
        {
            test_LU(A_1);
            test_LU(A_2);
            test_LU(A_3);
        }
        end = clock();
        cout << "LU time consuming: \n" << end - start << endl;

        start = clock();
        for (i = 0; i != 1000; i++)
        {
            test_LU_pp(A_1);
            test_LU_pp(A_2);
            test_LU_pp(A_3);
        }
        end = clock();
        cout << "LU_pp time consuming: \n" << end - start << endl;

        start = clock();
        for (i = 0; i != 1000; i++)
        {
            test_GJ(A_1);
            test_GJ(A_2);
            test_GJ(A_3);
        }
        end = clock();
        cout << "GJ time consuming: \n" << end - start << endl;

//        int iter = 1e4;
//        int pre = 3;
//        start = clock();
//        test_Jacobi(A_1, iter, pre);
//        test_Jacobi(A_2, iter, pre);
//        test_Jacobi(A_3, iter, pre);
//        end = clock();
//        cout << "Jocobi time consuming: \n" << end - start << endl;
//
//        start = clock();
//        for (i = 0; i != 1000; i++)
//        {
//            test_GS(A_1);
//            test_GS(A_2);
//            test_GS(A_3);
//        }
//        end = clock();
//        cout << "GS time consuming: \n" << end - start << endl;

        long double tol = 1e-6;
        start = clock();
        for (i = 0; i != 1000; i++)
        {
            test_CG(A_1, tol);
            test_CG(A_2, tol);
            test_CG(A_3, tol);
        }
        end = clock();
        cout << "CG time consuming: \n" << end - start << endl;
}

int * produce_random_matrix(int size, int min, int max)
{
    unsigned int seed = rand() % 100000;
    
    srand(seed);
    Matrix<int> A(size, size, true);
    for (int i = 0; i != size; i++)
        for (int j = 0; j != size; j++)
        {
            // cannot set initialized j = i, because the lower triangular value will not be set to zero
            if (i > j)
                A(i,j) = 0;
            else
            {
                A(i,j) = ( rand()%(max-min+1)  + min );
                if ( i == j && A(i,j) == 0 )
                    A(i,j) = max;
            }
        }
    Matrix<int> B(~A);
    Matrix<int> C(B*A);
    
    return C.values;
}

void test_random_matrix(int size, int min, int max)
{
    int n_n = size * size;
    
    int * val = produce_random_matrix(size, min, max);

    Matrix<double> A(size, size, true);
    for (int i = 0; i != n_n; i++)
        A.values[i] = val[i];
    
//    cout << "A is:\n" << A << endl;
    
    clock_t start, end;
    start = clock();
    test_GE(A);
    test_LU(A);
//    test_LU_pp(A);
//    test_Jacobi(A, 10000, 5);
    test_GJ(A);
    test_GS(A);
    test_CG(A, 1e-5);
    end = clock();
    cout << "time consuming for "<< size << " * " << size <<": \n" << end - start << endl;
}

double test_diefferent_size(void (*function)(Matrix<double> & A))
{
    vector<int> size_vec = {10,20,40,80,160,320,640};
    vector<Matrix<double>> mat_vec;
    vector<long int> time_vec;
    int min = -10;
    int max = 10;
    for (auto size: size_vec)
    {
        int n_n = size * size;
        int * val = produce_random_matrix(size, min, max);

        Matrix<double> A(size, size, true);
        for (int i = 0; i != n_n; i++)
            A.values[i] = val[i];
        mat_vec.push_back(A);
    }
    
    unsigned long time;
    clock_t start, end;

    for (auto mat: mat_vec)
    {
        start = clock();
        function(mat);
        end = clock();
        time = end - start;
        time_vec.push_back(time);
    }
    
    for (auto t: time_vec)
        cout << t << '\t';
    cout << endl;
    
    vector<double> time_mag;
    for (int i = 2; i != time_vec.size(); i++)
        time_mag.push_back( (double) time_vec[i] / time_vec[i-1] );
    
    // compute the average increase speed of the algorithm
    double sum = 0;
    for (auto t_m: time_mag)
        sum += t_m;
    
    double result = sum / (time_mag.size() - 1);
    
    return result;
}

void time_complexity()
{
    double t;
    cout << "GE time:\n";
    t = test_diefferent_size(test_GE);
    cout << t << endl;

    cout << "LU time:\n";
    t = test_diefferent_size(test_LU);
    cout << t << endl;
    
    cout << "LU_pp time:\n";
    t = test_diefferent_size(test_LU_pp);
    cout << t << endl;
    
    cout << "Gauss Jordan time:\n";
    t = test_diefferent_size(test_GJ);
    cout << t << endl;
}

void test_mb()
{
    for ( int qwe = 0; qwe != 1000; qwe++)
    {
    int size = 10;
    int min = -3;
    int max = 3;
    int n_n = size * size;
    int * val = produce_random_matrix(size, min, max);

    Matrix<double> A(size, size);
    for (int i = 0; i != n_n; i++)
        A.values[i] = val[i];
    
//    cout << A << endl;
    
    vector<double> b1(size);
    b1 = produce_b(A, min, max);
    
//    cout << b1 << endl;
    
    vector<double> b2(size);
    b2 = produce_b(A, min, max);
    
//    cout << b2 << endl;
    
    Matrix<double> b(size,2);
    for (int i = 0; i != size; i++)
    {
        b(i,0) = b1[i];
        b(i,1) = b2[i];
    }
    
        cout << "--------------------------------" << endl;
    Matrix<double> x(size,2);
    x = Gauss_Jordan(A, b);
    cout << x << endl;
    
    vector<double> x1(size);
    vector<double> x2(size);
    x1 = Gauss_Jordan(A, b1);
    x2 = Gauss_Jordan(A, b2);
    for (int i = 0; i != size; i++)
        cout << x1[i] << '\t' << x2[i] << endl;
    }
}
