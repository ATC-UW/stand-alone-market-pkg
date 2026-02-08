#pragma once
#include <cmath>
#include <limits>
#include <vector>

namespace indicators {

std::vector<float> sma(const std::vector<float>& prices, int period);
std::vector<float> ema(const std::vector<float>& prices, int period);
std::vector<float> rsi(const std::vector<float>& prices, int period);

struct MACDResult {
    std::vector<float> macd_line;
    std::vector<float> signal_line;
    std::vector<float> histogram;
};
MACDResult macd(const std::vector<float>& prices, int fast, int slow, int signal);

struct BollingerResult {
    std::vector<float> upper;
    std::vector<float> middle;
    std::vector<float> lower;
};
BollingerResult bollinger(const std::vector<float>& prices, int period, float std_dev);

std::vector<float> atr(const std::vector<float>& buy_prices,
                       const std::vector<float>& sell_prices, int period);

} // namespace indicators
