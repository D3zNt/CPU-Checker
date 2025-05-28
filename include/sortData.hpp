#ifndef SORT_DATA_HPP
#define SORT_DATA_HPP

#include <database.hpp>

void mergin(std::vector<CPU_DATA>& vec, int left, int mid, int right);
void sortmerger(std::vector<CPU_DATA>& vec, int left, int right);
#endif