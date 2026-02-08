#include "Indicator.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace indicators {

std::vector<float> sma(const std::vector<float>& prices, int period) {
    std::vector<float> result(prices.size(), std::numeric_limits<float>::quiet_NaN());
    if (period <= 0 || static_cast<int>(prices.size()) < period) {
        return result;
    }
    float sum = 0.0f;
    for (int i = 0; i < period; i++) {
        sum += prices[i];
    }
    result[period - 1] = sum / period;
    for (int i = period; i < static_cast<int>(prices.size()); i++) {
        sum += prices[i] - prices[i - period];
        result[i] = sum / period;
    }
    return result;
}

std::vector<float> ema(const std::vector<float>& prices, int period) {
    std::vector<float> result(prices.size(), std::numeric_limits<float>::quiet_NaN());
    if (period <= 0 || static_cast<int>(prices.size()) < period) {
        return result;
    }
    float sum = 0.0f;
    for (int i = 0; i < period; i++) {
        sum += prices[i];
    }
    float emaVal = sum / period;
    result[period - 1] = emaVal;
    float multiplier = 2.0f / (period + 1);
    for (int i = period; i < static_cast<int>(prices.size()); i++) {
        emaVal = (prices[i] - emaVal) * multiplier + emaVal;
        result[i] = emaVal;
    }
    return result;
}

std::vector<float> rsi(const std::vector<float>& prices, int period) {
    std::vector<float> result(prices.size(), std::numeric_limits<float>::quiet_NaN());
    if (period <= 0 || static_cast<int>(prices.size()) < period + 1) {
        return result;
    }
    float avgGain = 0.0f;
    float avgLoss = 0.0f;
    for (int i = 1; i <= period; i++) {
        float change = prices[i] - prices[i - 1];
        if (change > 0) avgGain += change;
        else avgLoss -= change;
    }
    avgGain /= period;
    avgLoss /= period;
    if (avgLoss == 0.0f) result[period] = 100.0f;
    else result[period] = 100.0f - 100.0f / (1.0f + avgGain / avgLoss);
    for (int i = period + 1; i < static_cast<int>(prices.size()); i++) {
        float change = prices[i] - prices[i - 1];
        float gain = change > 0 ? change : 0.0f;
        float loss = change < 0 ? -change : 0.0f;
        avgGain = (avgGain * (period - 1) + gain) / period;
        avgLoss = (avgLoss * (period - 1) + loss) / period;
        if (avgLoss == 0.0f) result[i] = 100.0f;
        else result[i] = 100.0f - 100.0f / (1.0f + avgGain / avgLoss);
    }
    return result;
}

MACDResult macd(const std::vector<float>& prices, int fast, int slow, int signal) {
    auto fastEma = ema(prices, fast);
    auto slowEma = ema(prices, slow);
    int n = static_cast<int>(prices.size());
    std::vector<float> macdLine(n, std::numeric_limits<float>::quiet_NaN());
    for (int i = slow - 1; i < n; i++) {
        if (!std::isnan(fastEma[i]) && !std::isnan(slowEma[i])) {
            macdLine[i] = fastEma[i] - slowEma[i];
        }
    }
    std::vector<float> macdValid;
    std::vector<int> macdIndices;
    for (int i = 0; i < n; i++) {
        if (!std::isnan(macdLine[i])) {
            macdValid.push_back(macdLine[i]);
            macdIndices.push_back(i);
        }
    }
    auto signalEma = ema(macdValid, signal);
    std::vector<float> signalLine(n, std::numeric_limits<float>::quiet_NaN());
    std::vector<float> histogram(n, std::numeric_limits<float>::quiet_NaN());
    for (int i = 0; i < static_cast<int>(macdValid.size()); i++) {
        if (!std::isnan(signalEma[i])) {
            int idx = macdIndices[i];
            signalLine[idx] = signalEma[i];
            histogram[idx] = macdLine[idx] - signalEma[i];
        }
    }
    return {macdLine, signalLine, histogram};
}

BollingerResult bollinger(const std::vector<float>& prices, int period, float std_dev) {
    int n = static_cast<int>(prices.size());
    auto middle = sma(prices, period);
    std::vector<float> upper(n, std::numeric_limits<float>::quiet_NaN());
    std::vector<float> lower(n, std::numeric_limits<float>::quiet_NaN());
    for (int i = period - 1; i < n; i++) {
        float sum = 0.0f;
        for (int j = i - period + 1; j <= i; j++) {
            float diff = prices[j] - middle[i];
            sum += diff * diff;
        }
        float sd = std::sqrt(sum / period);
        upper[i] = middle[i] + std_dev * sd;
        lower[i] = middle[i] - std_dev * sd;
    }
    return {upper, middle, lower};
}

std::vector<float> atr(const std::vector<float>& buy_prices,
                       const std::vector<float>& sell_prices, int period) {
    int n = static_cast<int>(buy_prices.size());
    std::vector<float> result(n, std::numeric_limits<float>::quiet_NaN());
    if (period <= 0 || n < period + 1) {
        return result;
    }
    std::vector<float> tr(n, 0.0f);
    tr[0] = buy_prices[0] - sell_prices[0];
    for (int i = 1; i < n; i++) {
        float highLow = buy_prices[i] - sell_prices[i];
        float highPrevClose = std::abs(buy_prices[i] - sell_prices[i - 1]);
        float lowPrevClose = std::abs(sell_prices[i] - sell_prices[i - 1]);
        tr[i] = std::max({highLow, highPrevClose, lowPrevClose});
    }
    float atrVal = 0.0f;
    for (int i = 0; i < period; i++) {
        atrVal += tr[i];
    }
    atrVal /= period;
    result[period - 1] = atrVal;
    for (int i = period; i < n; i++) {
        atrVal = (atrVal * (period - 1) + tr[i]) / period;
        result[i] = atrVal;
    }
    return result;
}

} // namespace indicators
