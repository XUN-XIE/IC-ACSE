//
//  main.cpp
//  A_4
//
//  Created by 谢逊 on 2020/1/25.
//  Copyright © 2020 谢逊. All rights reserved.
//

//Question A.4
//Write a recursive function to find the greatest common divisor of 2 integers (e.g. the greatest common divisor of 20 and 8 is 4). The recursive way to do this is by using something known as the Euclidian algorithm. The algorithm can be expressed as follows:
//The greatest common devisor of x and y is y if the remainder when x is divided by y is zero, otherwise it is the greatest common devisor of y and the remainder when x is divided by y (remember that the remainder is obtained using the % operator).
//Allow the user to enter a pair of numbers and display the greatest common divisor.


#include <iostream>
using namespace std;

int gcd(int a, int b);


int main()
{
    int a, b, c;
    cin >> a >> b;
    if ( a <= 0 || b <= 0 )
    {
        cerr << "Your integer should be positive.";
        exit(0);
    }
    c = gcd(a,b);
    cout << c << " is the greatest common divisor of " << a << " and " << b << endl;
}

int gcd(int a, int b)
{
    if ( b > a)
        swap(a,b);
    if ( b == 0 )
        return a;
    return gcd(b, a%b);
}
