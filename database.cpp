#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const int SIZE = 10;

struct CPU_DATA{
    int id;
    float cpu;    // percentage
    float memory; // percentage
};

void search();
void sort();
void convert_to_bson();
void convert_to_json();



int main(){
    const CPU_DATA tempList[10] = { // temporary later on we will try to connect with realtime data
        {1, 99.4, 85.3},
        {2, 12.3, 86.2},
        {3, 34.3, 73.2},
        {4, 12.2, 63.2},
        {5, 15.4, 23.2},
        {6, 32.3, 93.2},
        {7, 19.3, 23.2},
        {8, 93.3, 33.2},
        {9, 23.3, 3.2 },
        {10, 82.3, 8.2},
    };

    // FUNC list

    // simulated data (the CPU data will change in XX time, simulate real time)
    // search
    // sorting 
    // convert to BSON
    // convert to JSON
}




