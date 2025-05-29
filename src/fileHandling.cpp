#include <fileHandling.hpp>

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
        // Write the number of records
        size_t count = records.size();
        f.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& record : records) {
            size_t id_length = record.id.size();

            // Write the length of the ID string
            f.write(reinterpret_cast<const char*>(&id_length), sizeof(id_length));

            // Write the actual data of the ID
            f.write(record.id.data(), id_length);
            
            // Write the CPU, memory, timestamp values
            f.write(reinterpret_cast<const char*>(&record.cpu), sizeof(record.cpu));
            f.write(reinterpret_cast<const char*>(&record.memory), sizeof(record.memory));
            f.write(reinterpret_cast<const char*>(&record.timestamp), sizeof(record.timestamp));
        }
        std::cout << fileNameStr << " successfully created.";
        f.close();
    } else {
        std::cerr << "Failed to write file: " << fileNameStr << '\n';
    }
}

size_t deserializeBinaryFile(std::ifstream &in, double &fileCPUSum) {
    size_t logCount;
    in.read(reinterpret_cast<char*>(&logCount), sizeof(logCount));

    for (size_t i = 0; i < logCount; i++) {
        CPU_DATA record;

        // Read string length
        size_t id_length;
        in.read(reinterpret_cast<char*>(&id_length), sizeof(id_length));

        // Read string content
        record.id.resize(id_length);
        in.read(record.id.data(), id_length);

        in.read(reinterpret_cast<char*>(&record.cpu), sizeof(record.cpu));
        in.read(reinterpret_cast<char*>(&record.memory), sizeof(record.memory));
        in.read(reinterpret_cast<char*>(&record.timestamp), sizeof(record.timestamp));

        clientStats[record.id].first++;
        clientStats[record.id].second.push_back(record);

        // Calculate the CPU Usage sum for each file
        fileCPUSum += record.cpu;
    }
    return logCount;
}