#include "Utility.h"
#include <chrono>
#include <sstream>
#include <iomanip>

//Calculate the timestamp for each minute in the last 10 minutes
std::string subtractMinutes(const std::string& timestamp, int minutes) {
    std::tm tm = {};
    std::stringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y/%m/%d %H:%M:%S"); // Adjust the format if your timestamp format is different

    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    tp -= std::chrono::minutes(minutes);

    std::time_t time_t_tp = std::chrono::system_clock::to_time_t(tp);
    std::tm* tm_ptr = std::localtime(&time_t_tp);

    std::stringstream result;
    result << std::put_time(tm_ptr, "%Y/%m/%d %H:%M:%S"); // Use the same format as your input timestamp
    return result.str();
}