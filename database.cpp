#include <iostream>
#include <fstream>
#include <string>
#include <thread> 
#include <chrono>
#include <ctime>
#include <iomanip> 

using namespace std;

const int SIZE = 10;

struct CPU_DATA{
    int id;
    float cpu;    // percentage
    float memory; // percentage
};

void search();
void sort();
void convert_to_binary(CPU_DATA dataList[], int counter);
void convert_to_json();



int main(){
    CPU_DATA tempList[10] = { // temporary later on we will try to connect with realtime data
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


    // need multithreading  one thread is still here saving the file
    int counter = 1;
    while (true) {
        convert_to_binary(tempList, counter);
        counter++;
        std::this_thread::sleep_for(std::chrono::seconds(10));  
    }

    
}



void convert_to_binary(CPU_DATA dataList[],int counter){
    
    fstream f("records.dat", ios::out | ios::binary);

    if(f){
        f.write(reinterpret_cast<char*>(dataList), SIZE * sizeof(CPU_DATA));
        f.close();      
    }else {
        cout << "failed to write";
    }
    
    // f.open("records.dat", ios::in | ios::binary);
    // if(f){

    //     CPU_DATA temp[SIZE];
    //     f.read(reinterpret_cast<char*>(temp), SIZE * sizeof(CPU_DATA));
    //     f.close();


    //     for (CPU_DATA& r : temp){
    //         cout << r.cpu << ", " << r.id << ", " << r.memory << endl;
    //     }
    // }
}
