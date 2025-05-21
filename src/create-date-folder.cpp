#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <direct.h>
#include <string>
using namespace std;


ostringstream folderName;

int test() {


    time_t now = time(nullptr);
    tm local_tm;
#if defined(_WIN32) || defined(_WIN64) // this is used to check if the code running is in windows
    localtime_s(&local_tm, &now);
#else
    local_tm = *localtime(&now);
#endif
    
    folderName << put_time(&local_tm,"%d-%b-%Y" );
    const char* folderDir = folderName.str().c_str();  

    if (_mkdir(folderDir) == 0) {
        std::cout << "Folder created\n";
    } else {
        std::cout << "Failed to create folder or it exists\n";
    }
    return 0;
}