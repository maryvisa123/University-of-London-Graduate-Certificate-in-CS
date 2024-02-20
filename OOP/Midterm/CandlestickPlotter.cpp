#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <set>
#include <algorithm>
#include <cmath>
#include "CandlestickPlotter.h"

void plotCandlesticks(const std::vector<Candlestick>& candlesticks) {
    const int totalRows = 23;
    std::set<double> uniqueValues;

    // Collect unique values from each candlestick
    for (const auto& candle : candlesticks) {
        uniqueValues.insert(candle.Open);
        uniqueValues.insert(candle.High);
        uniqueValues.insert(candle.Low);
        uniqueValues.insert(candle.Close);
    }

    // Fill the rest of the scale marks
    while (uniqueValues.size() < (totalRows - 3)) {
        uniqueValues.insert(*uniqueValues.begin() - 0.0001); // Decrease smallest value
    }

    std::vector<double> scaleMarks(uniqueValues.begin(), uniqueValues.end());
    std::sort(scaleMarks.begin(), scaleMarks.end(), std::greater<double>());

    // Plotting
    for (int row = 0; row < totalRows; ++row) {
        if (row == 0 || row == totalRows - 2) {
            std::cout << std::string(65, '-') << std::endl;
            continue;
        }

        if (row == totalRows - 1) {
        // Printing timestamps at the x-axis
        int currentPos = 24;  // Start position for the first timestamp
        for (const auto& candle : candlesticks) {
            std::cout << std::setw(currentPos) << std::right << candle.Date.substr(14, 5);
            currentPos = 9;  // Subsequent timestamps are printed after 9 blank characters
        }
        std::cout << std::endl;
        continue;
        }

        // Scale marks with dynamic precision
        std::ostringstream scaleStream;
        scaleStream << std::fixed << std::setprecision(6) << scaleMarks[row - 1];
        std::string scaleStr = scaleStream.str();
        std::cout << std::setw(15) << scaleStr << " |";

        // Candlesticks
        for (const auto& candle : candlesticks) {
            auto findRow = [&](double value) {
                return std::lower_bound(scaleMarks.begin(), scaleMarks.end(), value, std::greater<double>()) - scaleMarks.begin() + 1;
            };

            int openPos = findRow(candle.Open);
            int highPos = findRow(candle.High);
            int lowPos = findRow(candle.Low);
            int closePos = findRow(candle.Close);

            // Determine the closest open/close position to high and low
            int closestToHigh = (std::abs(highPos - openPos) < std::abs(highPos - closePos)) ? openPos : closePos;
            int closestToLow = (std::abs(lowPos - openPos) < std::abs(lowPos - closePos)) ? openPos : closePos;

            bool isHighLowRow = (row == highPos) || (row == lowPos) || 
                                (row >= std::min(highPos, closestToHigh) && row <= std::max(highPos, closestToHigh)) ||
                                (row >= std::min(lowPos, closestToLow) && row <= std::max(lowPos, closestToLow));
            bool isOpenCloseRow = row == openPos || row == closePos;
            bool isBetweenOpenClose = row > std::min(openPos, closePos) && row < std::max(openPos, closePos);

            //Apply colors for clearer visualization
            std::string colorCode;
            if (candle.Open < candle.Close) {
                colorCode = "\033[32m"; // Green for Open < Close
            } else if (candle.Open > candle.Close) {
                colorCode = "\033[31m"; // Red for Open > Close
            } else {
                colorCode = "\033[0m";  // Default color
            }

            std::string candleRep = "         ";  // Default blank space

            if (isHighLowRow) {
                candleRep[4] = '|';  // High/Low mark in the middle
            }

            if (isOpenCloseRow || isBetweenOpenClose) {
                candleRep.replace(1, 7, "=======");
            }

            std::cout << colorCode << candleRep << "\033[0m"; // Apply color and reset after
        }
        std::cout << "|" << std::endl;
    }
}