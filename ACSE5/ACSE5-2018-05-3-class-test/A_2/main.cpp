//
//  main.cpp
//  A_2
//
//  Created by 谢逊 on 2020/1/25.
//  Copyright © 2020 谢逊. All rights reserved.
//


//Question A.2
//Write a program to calculate the value of the constant e. This can be done based on the Taylor series expansion:
//e=1+∑_(n=1)^∞▒1/n!
//where n! is the factorial of n (i.e. n!=1∙2∙3⋯n)
//Use a loop to calculate this value. This series converges quickly and so you need only go out to n = 20. The program should display both this calculated value of e, as well as that obtained using exp(1.0). Note that the factorial need not be calculated from scratch for each iteration of the loop as 1/n!=1/(n-1)!   1/n


#include <iostream>
#include <cmath>

using namespace std;

int main()
{
    double e = 1;
    double tmp = 1;
    for (int i = 1; i != 21; i++)
    {
        tmp *= 1.0 / i;
        e += tmp;
    }
    cout << "From taylor series, e is: " << e << endl;
    
    cout << "exact e is: " << exp(1.0) << endl;
}
