//
//  main.cpp
//  A_1
//
//  Created by 谢逊 on 2020/1/25.
//  Copyright © 2020 谢逊. All rights reserved.
//

//Question A.1
//Obtain a floating point number from the user. Ensure that the number is between -1 and 1 by obtaining the user input within a loop, with the user being asked for a new number until an appropriate one is provided. Calculate the arcsine of this number and display the answer in both radians and degrees (note that the asin function only returns the answer in radians and therefore you must convert it to degrees).
//#define _USE_MATH_DEFINES
//#include <iostream>
//#include <cmath>
//
//using namespace std;
//double theta;
//
//void main(void)
//{
//    cout << "Enter a number between 1 and -1 for a sine value:" << endl;
//    cin >> theta;
//    if (theta>=-1 && theta <=1)
//    {
//        cout << "The sine value you have entered leads to an angle of: " << asin(theta) << " radians,"<<endl;
//        cout << "or in: " << asin(theta)*360/M_PI << " degrees." << endl;
//    }
//    else (cout << "The value you have entered is not between 1 and -1!!!");
//}


#include <iostream>
#include <cmath>

using namespace std;



int main()
{
    double num;
    cout << "Please Enter a float number between -1 to 1: ";
    for (;;)
    {
        cin >> num;
        if (num <= 1 && num >= -1)
            break;
        cout << "Sorry, your input is wrong! Please Enter a float number between -1 to 1: ";
    }
    
    double arcsin = asin(num);
    cout << "radians of arcsin of your input is: " << arcsin << endl;
    double acrsin_degree = arcsin * 180 / 3.1415926;
    cout << "degrees of arcsin of your input is: " << acrsin_degree << endl;
}
