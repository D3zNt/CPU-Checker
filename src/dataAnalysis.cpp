#include <dataAnalysis.hpp>
#include <string>
#include <vector>
#include <sortData.hpp>
#include <cmath>
#include <algorithm>

json analyzeClientStats(double maxFileAvg, const std::string& maxFileName) {
    json clientsArray = json::array();
    double leastClientCount = std::numeric_limits<double>::infinity();
    std::string leastClient;
    int mostClientCount = -1;
    std::string topClient;

    for (const auto &entry : clientStats) {
        const std::string& client = entry.first;
        const auto& stats = entry.second;
        int numConnections = stats.first;
        auto records = stats.second;
        
        sortmerger(records, 0, records.size() - 1);
        
        double cpuSum = 0.0;
        double memorySum = 0.0;
        int dataCount = numConnections;
    
        json topUsageRecords = json::array();
        int topLimit = std::min(10, static_cast<int>(records.size()));
        for (int i = 0; i < topLimit; ++i) {
            cpuSum += records[i].cpu;
            memorySum += records[i].memory;
            topUsageRecords.push_back({
                {"cpu", records[i].cpu},
                {"memory", records[i].memory},
                {"timestamp", records[i].timestamp}
            });
        }
    
        json clientObj = {
            {"id", client},
            {"avg_cpu", cpuSum / dataCount},
            {"avg_memory", memorySum / dataCount},
            {"top_usage_records", topUsageRecords}
        };
    
        if (numConnections < leastClientCount) {
            leastClientCount = numConnections;
            leastClient = client;
        }
    
        if (numConnections > mostClientCount) {
            mostClientCount = numConnections;
            topClient = client;
        }
    
        clientsArray.push_back(clientObj);
    }
    
    json result;
    result["clients"] = clientsArray;
    result["highest_file_cpu_avg"] = {
        {"max_file_avg", maxFileAvg},
        {"filename", maxFileName}
    };
    result["least_connected_client"] = {
        {"no_of_connections", static_cast<int>(leastClientCount == std::numeric_limits<double>::infinity() ? 0 : leastClientCount)},
        {"client", leastClient}
    };
    result["most_connected_client"] = {
        {"no_of_connections", mostClientCount == -1 ? 0 : mostClientCount},
        {"client", topClient}
    };
    
    return result;
}
