#include <database.hpp>

std::vector<CPU_DATA> DATA_RECORDS; 
std::mutex dataMutex;
std::unordered_map<std::string, std::pair<int, std::vector<CPU_DATA>>> clientStats;