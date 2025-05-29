#ifndef DATA_ANALYSIS_HPP
#define DATA_ANALAYSIS_HPP

#include <nlohmann/json.hpp>
#include <database.hpp>

using json = nlohmann::json;

json analyzeClientStats(double maxFileAvg, const std::string& maxFileName);

#endif