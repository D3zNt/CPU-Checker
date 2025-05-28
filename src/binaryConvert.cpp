#include <binaryBackup.hpp>

std::string createFolder(std::string parentFolder) {
    _mkdir(parentFolder.c_str());

    std::ostringstream folderName;
    
    time_t now = time(nullptr);
    tm local_tm;
    localtime_s(&local_tm, &now);

    folderName << std::put_time(&local_tm, "%d-%b-%Y");
    std::string folderPath = parentFolder + '/' + folderName.str();
    
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
        size_t count = records.size();
        f.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& record : records) {
            size_t id_length = record.id.size();
            f.write(reinterpret_cast<const char*>(&id_length), sizeof(id_length));
            f.write(record.id.data(), id_length);
            
            f.write(reinterpret_cast<const char*>(&record.cpu), sizeof(record.cpu));
            f.write(reinterpret_cast<const char*>(&record.memory), sizeof(record.memory));
            f.write(reinterpret_cast<const char*>(&record.timestamp), sizeof(record.timestamp));

            std::cout << "COMPUTER ID: " << record.id << "\n";
            std::cout << "MEMORY USAGE (%): "<< record.memory << "\n";
            std::cout << "CPU USAGE (%): " << record.cpu << "\n";
            std::cout << "TIMESTAMP: " << record.timestamp << "\n\n";
        }

        // f.write(reinterpret_cast<const char*>(records.data()), records.size() * sizeof(CPU_DATA));
        f.close();
    } else {
        std::cerr << "Failed to write file: " << fileNameStr << '\n';
    }
}
