#ifndef VALIDATION_HPP
#define VALIDATION_HPP

#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

int validateJSON(json JSON);

#endif