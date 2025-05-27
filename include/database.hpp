#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <vector>
#include <string>
#include <ctime>
#include <mutex>

typedef struct {
    std::string id;
    time_t timestamp;
    double cpu;    // percentage
    double memory; // percentage
} CPU_DATA;

extern std::mutex dataMutex;
extern std::vector<CPU_DATA> DATA_RECORDS;

#endif