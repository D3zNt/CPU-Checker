#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <vector>
#include <string>
#include <ctime>
#include <unordered_map>
#include <mutex>

typedef struct {
    std::string id;
    time_t timestamp;
    double cpu;    // percentage
    double memory; // percentage
} CPU_DATA;

struct DateTimeInput {
    int day;
    std::string month;
    int year;
    int startHour;
    int startMinute;
    int endHour;
    int endMinute;
};

extern std::mutex dataMutex;
extern std::vector<CPU_DATA> DATA_RECORDS;

// Store total CPU usage and count of appearances for each client
extern std::unordered_map<std::string, std::pair<int, std::vector<CPU_DATA>>> clientStats;

#endif