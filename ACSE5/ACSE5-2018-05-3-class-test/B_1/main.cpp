//
//  main.cpp
//  B_1
//
//  Created by 谢逊 on 2020/1/25.
//  Copyright © 2020 谢逊. All rights reserved.
//


//Question B.1
//Create a class which stores the name of a location together with its latitude and longitude. Write a member function that reads in the information from the user, with the latitude and longitude being obtained in degrees, but then also stored as radians (i.e. two member variables for both latitude and longitude). Write another member function which displays the name and location in both radians and degrees.
//Write a further member function which takes in an objects of the class you have created and which returns the great circle distance from the original object to the object being passed to the function. You can use the following equations to calculate this distance:
//a=〖sin〗^2 ((φ_2-φ_1)/2)+cos(φ_1 )cos(φ_2 ) 〖sin〗^2 ((λ_2 〖-λ〗_1)/2)
//θ=2 atan2(√a,√(1-a))
//    Distance= θ R
//Where φ is the latitude, λ is the longitude and R is the radius of the earth (6 371 km).   Note that atan2 returns the inverse tan, but does so based on the opposite and adjacent edge lengths rather than their ratio (this ensures that the quadrant for the angle is correct). Also note that the sines are squared.
//In the main body of the code allow the user to specify how many locations they wish to enter and then use “new” to dynamically allocate an array of the location objects. The user should then be able to enter the details for each of the objects using the appropriate member function.
//The code must then display each possible pair of locations, using the member function to display the names and coordinates, together with the distance between them. To avoid duplicating pairs you should loop over all the locations and then within that loop you should loop over all locations with an index higher than that of the first location being considered.
//Remember to free the memory before the program exits.


#include <iostream>
#include <cmath>


using namespace std;

const double degree_to_radians = 3.1415926 / 180.0;
const double radians_to_degree = 180.0 / 3.1415926;
const double R = 6371;

class location
{
private:
    double lat;   // radians
    double lon;   // radians
    char name[100];
public:
    location() {}
    void input();
    void display() const;
    friend double distance(const location & loc1, const location & loc2);
};

int main()
{
    int num;
    cout << "Please input the number of objects you want to create: ";
    cin >> num;
    
    location * p_loc = new location [num];
    for (int i = 0; i != num; i++)
    {
        cout << "For " << i+1 << " object: " << endl;
        p_loc[i].input();
    }
    
    double dist;
    for (int i = 0; i != num; i++)
    {
        for ( int j = i + 1; j != num; j++)
        {
            cout << "the objects you compare is: " << i+1 << " and " << j+1 << endl;
            p_loc[i].display();
            p_loc[j].display();
            dist = distance(p_loc[i], p_loc[j]);
            cout << "the distance between " << i+1 << " and " << j+1 << " is: " << dist << endl << endl;
        }
    }
    
    delete [] p_loc;
}

void location::input()
{
    cout << "Input the name of the object: ";
    cin.ignore();                                        // 这一行不明白，所以最开始没这一行，出现了读取输入错误！加强输入输出流的学习，考试貌似每一题都要用到
    cin.getline(name, 100);
    
    cout << "Input the latitude and longitude in degree form." << endl;
    double num1, num2;
    cin >> num1 >> num2;
    lat = num1 * degree_to_radians;
    lon = num2 * degree_to_radians;
}

void location::display() const
{
    cout << "the name of the object is: " << name << endl;
    cout << "the latitude and longitude of the location in radians is: (" << lat << ", " << lon << ")" << endl;
    cout << "the latitude and longitude of the location in degree is: (" << lat * radians_to_degree << ", " << lon * radians_to_degree << ")" << endl;
}

double distance(const location & loc1, const location & loc2)
{
    double num1 = (loc1.lat - loc2.lat) / 2;
    double num2 = (loc1.lon - loc2.lon) / 2;
    double a = pow(sin(num1),2) + cos(loc1.lat) * cos(loc1.lat) * pow( sin(num2), 2 );
    
    double cita = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return cita * R;
}


