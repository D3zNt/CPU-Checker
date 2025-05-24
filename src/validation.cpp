#include <validation.hpp>

using json = nlohmann::json;

int validateJSON(json JSON) {
    if (!JSON.contains("id") || !JSON.contains("cpu_usage") || !JSON.contains("memory_usage") || !JSON.contains("timestamp")) {
        std::cerr << "Missing required fields\n";
        return -1;
    }

    if (!JSON["cpu_usage"].is_number() || !JSON["memory_usage"].is_number()) {
        std::cerr << "Invalid field types\n";
        return -1;
    }

    if ((JSON["cpu_usage"] < 0 || JSON["cpu_usage"] > 100) || 
        (JSON["memory_usage"] < 0 || JSON["memory_usage"] > 100)) {
        std::cerr << "Data out of range\n";
        return -1;
    }
    return 0;
}