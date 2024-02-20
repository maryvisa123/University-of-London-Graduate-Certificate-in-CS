#include "MovingAveragePlotter.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <sstream>

void plotMovingAverages(const std::vector<double>& movingAverages, const std::vector<std::string>& timestamps) {
    const int totalRows = 23;
    std::vector<double> uniqueValues(movingAverages.begin(), movingAverages.end());

    // Ensure the unique values list is complete for the scale
    while (uniqueValues.size() < (totalRows - 3)) {
        uniqueValues.push_back(*uniqueValues.rbegin() - 0.0001); // Decrease smallest value
    }

    std::sort(uniqueValues.begin(), uniqueValues.end(), std::greater<double>());

    // Plotting
    for (int row = 0; row < totalRows; ++row) {
        if (row == 0 || row == totalRows - 2) {
            std::cout << std::string(108, '-') << std::endl;
            continue;
        }

        if (row == totalRows - 1) {
            // Printing timestamps at the x-axis
            int currentPos = 24;  // Start position for the first timestamp
            for (const auto& timestamp : timestamps) {
                std::cout << std::setw(currentPos) << std::right << timestamp.substr(11, 5);
                currentPos = 9;  // Space between timestamps
            }
            std::cout << std::endl;
            continue;
        }

        // Scale marks with dynamic precision
        std::ostringstream scaleStream;
        scaleStream << std::fixed << std::setprecision(6) << uniqueValues[row - 1];
        std::string scaleStr = scaleStream.str();
        std::cout << std::setw(15) << scaleStr << " |";

        // Moving Averages
        for (const auto& avg : movingAverages) {
            int avgPos = std::lower_bound(uniqueValues.begin(), uniqueValues.end(), avg, std::greater<double>()) - uniqueValues.begin() + 1;

            std::string avgRep = "         ";  // Default blank space
            if (row == avgPos) {
                avgRep[4] = '*';  // Mark the average position
            }

            std::cout << avgRep;
        }
        std::cout << "|" << std::endl;
    }
}