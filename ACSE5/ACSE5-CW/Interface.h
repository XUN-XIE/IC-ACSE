#pragma once
#include <string>
#include <fstream>
#include <cstdio>
#include <vector>
#include <iostream>

using namespace std;

class Interface
{

public:
    Interface();
    ~Interface();

private:

    // INTERFACE (terminal prompt) FUNCTIONS
    void interfaceIntro();
    
    // choose dense or sparse matrix
    void interfaceSelectMatrix();
    
    // change the random matrix or fix matrix continusely?
    void change_b_Matrix(int choice);
    
    // choose 4 dense algorithm
    void interfaceDenseMatrix();
    
    // choose 3 sparse algorithm
    void interfaceSparseMatrix();
    
    // if interface is invalid (wrong input), showing error
    void interfaceInvalid(string message);
    
    // run different dense algo.
    void denseAlgorithm(char select_char);
    
    // run different sparse algo.
    void sparseAlgorithm(char select_char);
    
    // load matrix data
    int loadMatrixData();
};

