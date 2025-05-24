#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <vector>
#include <string>
#include <ctime>

typedef struct {
    std::string id;
    time_t timestamp;
    float cpu;    // percentage
    float memory; // percentage
} CPU_DATA;

extern std::vector<CPU_DATA> DATA_RECORDS;

#endif