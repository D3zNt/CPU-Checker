#include <iostream>
#include <fstream>
#include <string>
#include <thread> 
#include <chrono>
#include <ctime>
#include <iomanip> 
#include "create-date-folder.cpp"

using namespace std;

const int SIZE = 10;

struct CPU_DATA{
    int id;
    float cpu;    // percentage
    float memory; // percentage
};

void search();
void sort();
void convert_to_binary(CPU_DATA dataList[]);
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

    test();


    // need multithreading  one thread is still here saving the file

    while (true) {
        convert_to_binary(tempList);

        std::this_thread::sleep_for(std::chrono::seconds(10));  
    }

    
}



void convert_to_binary(CPU_DATA dataList[]){

    ostringstream fileName;
    time_t now = time(nullptr);
    tm local_tm;

#if defined(_WIN32) || defined(_WIN64) // this is used to check if the code running is in windows
    localtime_s(&local_tm, &now);
#else
    local_tm = *localtime(&now);
#endif
    
    fileName << folderName.str() << "/backup[" << put_time(&local_tm,"%H-%M-%S" ) << "].dat";
    const char* fileNameInStr = fileName.str().c_str();

    fstream f(fileNameInStr, ios::out | ios::binary);

    if(f){
        f.write(reinterpret_cast<char*>(dataList), SIZE * sizeof(CPU_DATA));
        f.close();      
    }else {
        cout << "failed to write";
    }
    
}
