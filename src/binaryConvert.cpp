#include <binaryBackup.hpp>

std::ostringstream folderName;

int createFolder() {

    time_t now = time(nullptr);
    tm local_tm;

    localtime_s(&local_tm, &now);

    folderName << std::put_time(&local_tm,"%d-%b-%Y" );
    const char* folderDir = folderName.str().c_str();  

    if (_mkdir(folderDir) == 0) {
        std::cout << "Folder created\n";
    } else {
        std::cout << "Failed to create folder or it exists\n";
    }
    return 0;
}

void convert_to_binary(){

    std::ostringstream fileName;
    time_t now = time(nullptr);
    tm local_tm;

    localtime_s(&local_tm, &now);

    
    fileName << folderName.str() << "/backup[" << std::put_time(&local_tm,"%H-%M-%S" ) << "].dat";
    const char* fileNameInStr = fileName.str().c_str();

    std::fstream f(fileNameInStr, std::ios::out | std::ios::binary);

    if(f){
        f.write(reinterpret_cast<char*>(DATA_RECORDS.data()),DATA_RECORDS.size() * sizeof(CPU_DATA) );
        f.close();      
    }else {
        std::cout << "failed to write";
    }
    
}