#include <binaryBackup.hpp>

std::string createFolder() {
    std::ostringstream folderName;
    
    time_t now = time(nullptr);
    tm local_tm;
    localtime_s(&local_tm, &now);

    folderName << std::put_time(&local_tm, "%d-%b-%Y");
    std::string folderPath = folderName.str();
    
    if (_mkdir(folderPath.c_str()) == 0) {
        std::cout << "Folder created\n";
    } else {
        std::cout << "Failed to create folder or it exists\n";
    }

    return folderPath;
}
void convert_to_binary(const std::vector<CPU_DATA>& records, const std::string& timeframe, const std::string& folderPath) {
    std::ostringstream fileName;

    fileName << "./" << folderPath << "/backup[" << timeframe << "].dat";

    std::string fileNameStr = fileName.str();

    std::fstream f(fileNameStr, std::ios::out | std::ios::binary);
    if (f) {
        for (auto &v : records) {
            std::cout << "COMPUTER ID: " << v.id << "\n";
            std::cout << "MEMORY USAGE (%): "<< v.memory << "\n";
            std::cout << "CPU USAGE (%): " << v.cpu << "\n";
            std::cout << "TIMESTAMP: " << v.timestamp << "\n\n";
        }
        f.write(reinterpret_cast<const char*>(records.data()), records.size() * sizeof(CPU_DATA));
        f.close();
    } else {
        std::cerr << "Failed to write file: " << fileNameStr << '\n';
    }
}
