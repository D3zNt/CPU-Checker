#ifndef DATABASE_HPP
#define DATABASE_HPP
#pragma once

#include <vector>

typedef struct {
    int id;
    float cpu;    // percentage
    float memory; // percentage
} CPU_DATA;

extern std::vector<CPU_DATA> DATA_RECORDS;

#endif