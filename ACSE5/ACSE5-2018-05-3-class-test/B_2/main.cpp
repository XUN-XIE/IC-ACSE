//
//  main.cpp
//  B_2
//
//  Created by 谢逊 on 2020/1/25.
//  Copyright © 2020 谢逊. All rights reserved.
//

//A Perfect number is one where the sum of the factors of that number add up to the number itself. For instance 28 is a Perfect number as its factors are 1, 2, 4, 7 and 14, which add up to 28. Write a computer program that finds and displays all the Perfect numbers below 10 000 (there are only 4 of them) and also displays their factors. The code should also write this data to file.
//To find the Perfect numbers you must test every number less than 10 000. For each number being tested you must find all of its factors, which involves checking all the numbers less than the one being tested to see if they are a factor (i.e. have a remainder of zero when the number being tested is divided by this number). All these factors should be summed and the sum then checked to see if it adds up to the number being tested.
//To display the factors you must store them in an array as they are found, but only print them out if the number being tested is a Perfect number (note that there are other ways to display the factors, but this is the method that I wish you to use).
//Remember to also write all the Perfect numbers found, as well as their factors, to a single text file.


#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>

using namespace std;

vector<int> factors(int num);

int main()
{
    vector<int> perfect = {};
    for ( int i = 1; i != 10001; i++ )
    {
        vector<int> tmp = factors(i);
        if ( i == accumulate( tmp.begin(), tmp.end(), 0) )
            perfect.push_back(i);
    }
    
    for (auto x: perfect)
        cout << x << '\t';
    cout << endl;
                                                            // 缺乏error handling机制，如果文件打不开怎么办？
    ofstream outfile;                                       // 读入打开文件忘记了，头文件是<fstream>，但是创建对象用 ofstream 声明和定义;
    outfile.open("perfect_numbers.txt");
    if ( outfile.fail() )
    {
        cout << "Error opening file!!" << endl;
        exit(0);
    }
    
    for (auto x: perfect)
        outfile << x << '\t';
    
    outfile.close();
    cout << "end\n";
}

vector<int> factors(int num)
{
    vector<int> res = {};
    for (int i = 1; i != num / 2 + 1; i++)
    {
        if ( num % i == 0)
            res.push_back(i);
    }
    return res;
}
