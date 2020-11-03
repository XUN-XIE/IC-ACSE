//
//  main.cpp
//  A_3
//
//  Created by 谢逊 on 2020/1/25.
//  Copyright © 2020 谢逊. All rights reserved.
//

//Question A.3
//Write a function that takes in a string and then checks if the string is a palindrome, returning true if it is a palindrome, otherwise returning false. A palindrome is a word or sentence that is spelt the same forwards or backwards (e.g. "anna", "now I won" or "able was I ere I saw elba"). To do this you need to loop over half the string and check if the ith character is the same as the (string length-1-i)th character (note that the -1 is because C/C++ arrays are zero based).
//In the main function read in the string from the user and use the function to check if the string is a palindrome, writing an appropriate comment to screen.


#include <iostream>
#include <string>

using namespace std;

bool palindrome(const string & str);

int main()
{
    string str;
    cin >> str;
    bool val;
    val = palindrome(str);
    
    if (val)
        cout << "Your input is a palindrome." << endl;
    else
        cout << "Your input is not a palindrome." << endl;
}

bool palindrome(const string & str)
{
    for (int i = 0; i != str.size()/2; i++)
    {
        if (str[i] != str[str.size() - i - 1])
            return false;
    }
    return true;
}
