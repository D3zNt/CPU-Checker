#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <map>
#include <nlohmann/json.hpp>
#include <vector>
#include <sortData.hpp>
#include <cmath>
#include <database.hpp>
#include <Windows.h>
#include <direct.h>

using json = nlohmann::json;

// Function to check if the file's timestamp is within the requested time range
bool isInTimeRange(const std::string& filename, int startSec, int endSec) {
    size_t startPos = filename.find("backup[");
    size_t endPos = filename.find("].dat");

    if (startPos == std::string::npos || endPos == std::string::npos)
        return false;

    std::string timeStr = filename.substr(startPos + 7, endPos - (startPos + 7)); // Extract "HH-MM"
    int h, m;
    sscanf(timeStr.c_str(), "%2d-%2d", &h, &m);
    int totalSec = h * 3600 + m * 60;

    return totalSec >= startSec && totalSec <= endSec;
}


int main() {
    json j;

    std::string month;
    int date, year;
    std::cout << "Enter DD MM YY: ";
    std::cin >> date >> month >> year;

    int sh, sm, eh, em;
    std::cout << "Enter start time (HH MM): ";
    std::cin >> sh >> sm;
    std::cout << "Enter end time (HH MM): ";
    std::cin >> eh >> em;

    int startSec = sh * 3600 + sm * 60;
    int endSec = eh * 3600 + em * 60;

    // Store total CPU usage and count of appearances for each client
    std::unordered_map<std::string, std::pair<int, std::vector<CPU_DATA>>> clientStats;

    double maxFileAvg = -1.0;
    std::string maxFileName;
    int filesRead = 0;

    std::string strPath = "Backup/" + std::to_string(date) + '-' + month + '-' + std::to_string(year) + '/';
    std::filesystem::path dataPath = strPath;

    if (!std::filesystem::exists(dataPath) || !std::filesystem::is_directory(dataPath)) {
        std::cerr << "Error: The directory \"" << strPath << "\" does not exist or is not a valid directory.\n";
        return 1;
    }
    // Monitor the client that connects the least and most
    double leastClientCount = std::numeric_limits<double>::infinity();
    std::string leastClient;
    
    int mostClientCount = -1;
    std::string topClient;

    for (std::filesystem::directory_iterator dirIt(dataPath); dirIt != std::filesystem::directory_iterator(); ++dirIt) {
        std::filesystem::directory_entry entry = *dirIt;
        std::string fname = entry.path().filename().string();

        if (!entry.is_regular_file() || fname.find("backup[") == std::string::npos) {
            continue;
        }

        // Only include files within the time range
        if (!isInTimeRange(fname, startSec, endSec)) {
            continue;
        }        
        std::ifstream in(entry.path(), std::ios::binary);
        if (!in) {
            continue;
        }

        size_t logCount;
        in.read(reinterpret_cast<char*>(&logCount), sizeof(logCount));

        double fileSum = 0.0;
        int fileCount = 0;

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
            fileSum += record.cpu;
        }

        // Track the file with the highest average CPU usage
        double avg = fileSum / logCount;
        if (avg > maxFileAvg) {
            maxFileAvg = avg;
            maxFileName = fname;
        }

        filesRead++;
    }

    if (filesRead == 0) {
        std::cout << "No files found in the time range.\n";
        return 0;
    }

    json clientsArray = json::array();
    for (auto it = clientStats.begin(); it != clientStats.end(); ++it) {
        std::string client = it->first;
        int ClientNoOfConnections = it->second.first;

        sortmerger(it->second.second, 0, it->second.second.size() - 1);

        double cpuSum = 0.0;
        double memorySum = 0.0;
        int dataCount = it->second.first;

        json topUsageRecords = json::array();
        int topLimit = min(10, it->second.second.size());
        for (int i = 0; i < topLimit; ++i) {
            auto &vec = it->second.second[i];
            cpuSum += vec.cpu;
            memorySum += vec.memory;
            topUsageRecords.push_back({
                {"cpu", vec.cpu},
                {"memory", vec.memory},
                {"timestamp", vec.timestamp}
            });
        }

        json clientObj = {
            {"id", client},
            {"avg_cpu", cpuSum / dataCount},
            {"avg_memory", memorySum / dataCount},
            {"top_usage_records", topUsageRecords}
        };

        if (ClientNoOfConnections < leastClientCount) {
            leastClient = client;
            leastClientCount = ClientNoOfConnections;
        }

        if (ClientNoOfConnections > mostClientCount) {
            mostClientCount =  ClientNoOfConnections;
            topClient = client;  
        }
        clientsArray.push_back(clientObj);
    }

    j["clients"] = clientsArray;

    j["highest_file_cpu_avg"] = {
        {"max_file_avg", maxFileAvg},
        {"filename", maxFileName}
    };

    if (std::isinf(leastClientCount)) leastClientCount = 0.0f;
    j["least_connected_client"] = {
        {"no_of_connections", static_cast<int>(leastClientCount)},
        {"client", leastClient}
    };

    if (mostClientCount == -1) mostClientCount = 0;
    j["most_connected_client"] = {
        {"no_of_connections", mostClientCount},
        {"client", topClient}
    };

    _mkdir("Analysis");
    std::string analysisFilename = "Analysis/analysis[" + std::to_string(sh) + '-' + std::to_string(sm) + '_' + std::to_string(eh) + '-' + std::to_string(em) + "].json";
    std::ofstream ofile(analysisFilename);
    
    ofile << std::setw(4) << j << std::endl;
    if (!ofile.is_open()) {
        std::cout << "Failed to create file: " << analysisFilename << "\n";
        perror("Reason");
        return 1;
    }
    return 0;
}