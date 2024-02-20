#pragma once

#include <string>
#include <vector>

class Candlestick {
public:
    std::string Date;
    double Open;
    double High;
    double Low;
    double Close;

    Candlestick(std::string Date, double open, double high, double low, double close);
};

