//
//  main.cpp
//  B_3
//
//  Created by 谢逊 on 2020/1/25.
//  Copyright © 2020 谢逊. All rights reserved.
//

//Question B.3
//Write a program to calculate the standard deviation of a list of numbers. The numbers should be stored as a linked list. The program must NOT ask how many items are to be added beforehand, but should instead add numbers to the list until the user indicates that they do not wish to add any more numbers.
//Write a function that takes in a pointer to the first item in the list and returns the standard deviation of the list.  Use the following formula for the standard deviation, using a loop to obtain the required sums (N being the number of items in the list and x_i being the ith item, though as this is a linked list, the items won’t actually have an index):
//σ=√(1/N ∑_(i=0)^(N-1)▒〖x_i〗^2 -(1/N ∑_(i=0)^(N-1)▒x_i )^2 )
//The program should use this function to calculate the standard deviation of the list and display it.
//Once the program has displayed the standard deviation of the list, the user should be given the option to add more numbers to the list, clear the list and add new numbers or to exit the program. Remember to free all memory before the program exits.

#include <iostream>
#include <list>
#include <cmath>
#include <string>

using namespace std;

double deviation(list<double> * p);

int main()
{
    list<double> lis;
    cout << "Please input the numbers you want to calculate, you can quit by input character: ";
    double tmp;
    while (cin >> tmp)
        lis.push_back(tmp);
    
    double dev;
    dev = deviation(&lis);
    cout << "the deviation of your input is: " << dev << endl;
    
    cout << "You can decide whether or not to continue.\nIf you want to empty list and restart please input \"restart\". If you want to add new number, please input \"add\"." << endl;
    cout << "Or you can input \"end\" to stop" << endl;
    string choice;
//    cin.get();                      // 这里读取choice有问题，电脑会自动读取上面的输出后面的，同理，while循环里也是，所以变成无限死循环了。
    
    string temp;
    cin.clear();
    cin >> temp;
    cin >> choice;
    int count = 0;
    while ( choice != "end" && count != 2)
    {
        count++;
        if (choice == "restart")
        {
            lis = {};
            cout << "Please input the numbers you want to calculate, you can quit by input character: ";
            double tmp;
            while (cin >> tmp)
                lis.push_back(tmp);
            
            double dev;
            dev = deviation(&lis);
            cout << "the deviation of your input is: " << dev << endl;
            
            cout << "You can decide whether or not to continue.\nIf you want to empty list and restart please input \"restart\". If you want to add new number, please input \"add\"." << endl;
            cout << "Or you can input \"end\" to stop" << endl;
            cin.clear();
            cin >> temp;
            cin >> choice;
            
        }
        else if(choice == "add")
        {
            cout << "Please input the numbers you want to calculate, you can quit by input character: ";
            double tmp;
            while (cin >> tmp)
                lis.push_back(tmp);
            
            double dev;
            dev = deviation(&lis);
            cout << "the deviation of your input is: " << dev << endl;
            
            cout << "You can decide whether or not to continue.\nIf you want to empty list and restart please input \"restart\". If you want to add new number, please input \"add\"." << endl;
            cout << "Or you can input \"end\" to stop" << endl;
            cin.clear();
            cin >> temp;
            cin >> choice;
        }
        else
        {
            cout << "Input is: " << choice << endl << endl;
            cout << "Sorry! You input a wrong choice, please input your choice again!" << endl;
            cout << "You can decide whether or not to continue.\nIf you want to empty list and restart please input \"restart\". If you want to add new number, please input \"add\"." << endl;
            cout << "Or you can input \"end\" to stop" << endl;
            cin.clear();
            cin >> temp;
            cin >> choice;
        }
    }
    
}

double deviation(list<double> * p)
{
    double sum_x_2 = 0;
    double sum_x = 0;
    for (auto x: *p)
    {
        sum_x_2 += x * x;
        sum_x += x;
    }
    
    return sqrt(sum_x_2 / p->size() - pow(sum_x / p->size(), 2) );
}
