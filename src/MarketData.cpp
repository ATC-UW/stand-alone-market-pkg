#include "MarketData.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <functional>
#include <stdexcept>

MarketData::MarketData(float startBuyPrice, float startSellPrice,
                       std::vector<RegimeAssignment> regimes,
                       std::optional<unsigned int> seed) {
  if (seed.has_value()) {
    rng.seed(seed.value());
  } else {
    rng.seed(static_cast<unsigned int>(
        std::chrono::steady_clock::now().time_since_epoch().count()));
  }

  totalDays = 0;
  for (const auto &assignment : regimes) {
    if (assignment.endDay > totalDays) {
      totalDays = assignment.endDay;
    }
  }

  dayRegimes.resize(totalDays, nullptr);
  for (const auto &assignment : regimes) {
    for (int d = assignment.startDay; d < assignment.endDay; d++) {
      dayRegimes[d] = assignment.regime;
    }
  }

  buyPrices.push_back(startBuyPrice);
  sellPrices.push_back(startSellPrice);

  computePrices();

  // Compute mid prices
  midPrices.resize(buyPrices.size());
  for (size_t i = 0; i < buyPrices.size(); i++) {
    midPrices[i] = (buyPrices[i] + sellPrices[i]) / 2.0f;
  }
}

void MarketData::computePrices() {
  for (int i = 0; i < totalDays; i++) {
    if (dayRegimes[i]) {
      dayRegimes[i]->setDayIndex(i);
      float newBuy = dayRegimes[i]->update(buyPrices.back(), rng);
      float newSell = dayRegimes[i]->update(sellPrices.back(), rng);
      // Enforce ask >= bid (buy price >= sell price)
      if (newSell > newBuy) {
        std::swap(newBuy, newSell);
      }
      buyPrices.push_back(newBuy);
      sellPrices.push_back(newSell);
    } else {
      buyPrices.push_back(buyPrices.back());
      sellPrices.push_back(sellPrices.back());
    }
  }
}

std::vector<float> MarketData::getBuyPrices(int start, int end) {
  if (end == -1) {
    end = static_cast<int>(buyPrices.size());
  }
  if (start < 0 || end > static_cast<int>(buyPrices.size()) || start >= end) {
    throw std::out_of_range("Invalid day range");
  }
  return std::vector<float>(buyPrices.begin() + start,
                            buyPrices.begin() + end);
}

std::vector<float> MarketData::getSellPrices(int start, int end) {
  if (end == -1) {
    end = static_cast<int>(sellPrices.size());
  }
  if (start < 0 || end > static_cast<int>(sellPrices.size()) || start >= end) {
    throw std::out_of_range("Invalid day range");
  }
  return std::vector<float>(sellPrices.begin() + start,
                            sellPrices.begin() + end);
}

int MarketData::getTotalDays() { return totalDays; }

std::vector<float> MarketData::getMidPrices(int start, int end) {
  if (end == -1) {
    end = static_cast<int>(midPrices.size());
  }
  if (start < 0 || end > static_cast<int>(midPrices.size()) || start >= end) {
    throw std::out_of_range("Invalid day range");
  }
  return std::vector<float>(midPrices.begin() + start,
                            midPrices.begin() + end);
}

std::vector<float> MarketData::sliceResult(const std::vector<float>& data,
                                           int start, int end) {
  if (end == -1) {
    end = static_cast<int>(data.size());
  }
  if (start < 0 || end > static_cast<int>(data.size()) || start >= end) {
    throw std::out_of_range("Invalid day range");
  }
  return std::vector<float>(data.begin() + start, data.begin() + end);
}

const std::vector<float>& MarketData::getCachedOrCompute(
    const std::string& key,
    const std::vector<float>& prices,
    std::function<std::vector<float>(const std::vector<float>&)> computeFn) {
  auto it = indicatorCache.find(key);
  if (it == indicatorCache.end()) {
    indicatorCache[key] = computeFn(prices);
    return indicatorCache[key];
  }
  return it->second;
}

// SMA
std::vector<float> MarketData::getBuySMA(int period, int start, int end) {
  std::string key = "buy_sma_" + std::to_string(period);
  const auto& data = getCachedOrCompute(key, buyPrices,
      [period](const std::vector<float>& p) { return indicators::sma(p, period); });
  return sliceResult(data, start, end);
}
std::vector<float> MarketData::getSellSMA(int period, int start, int end) {
  std::string key = "sell_sma_" + std::to_string(period);
  const auto& data = getCachedOrCompute(key, sellPrices,
      [period](const std::vector<float>& p) { return indicators::sma(p, period); });
  return sliceResult(data, start, end);
}

// EMA
std::vector<float> MarketData::getBuyEMA(int period, int start, int end) {
  std::string key = "buy_ema_" + std::to_string(period);
  const auto& data = getCachedOrCompute(key, buyPrices,
      [period](const std::vector<float>& p) { return indicators::ema(p, period); });
  return sliceResult(data, start, end);
}
std::vector<float> MarketData::getSellEMA(int period, int start, int end) {
  std::string key = "sell_ema_" + std::to_string(period);
  const auto& data = getCachedOrCompute(key, sellPrices,
      [period](const std::vector<float>& p) { return indicators::ema(p, period); });
  return sliceResult(data, start, end);
}

// RSI
std::vector<float> MarketData::getBuyRSI(int period, int start, int end) {
  std::string key = "buy_rsi_" + std::to_string(period);
  const auto& data = getCachedOrCompute(key, buyPrices,
      [period](const std::vector<float>& p) { return indicators::rsi(p, period); });
  return sliceResult(data, start, end);
}
std::vector<float> MarketData::getSellRSI(int period, int start, int end) {
  std::string key = "sell_rsi_" + std::to_string(period);
  const auto& data = getCachedOrCompute(key, sellPrices,
      [period](const std::vector<float>& p) { return indicators::rsi(p, period); });
  return sliceResult(data, start, end);
}

// MACD
std::tuple<std::vector<float>, std::vector<float>, std::vector<float>>
MarketData::getBuyMACD(int fast, int slow, int signal, int start, int end) {
  std::string base = "buy_macd_" + std::to_string(fast) + "_" +
                     std::to_string(slow) + "_" + std::to_string(signal);
  std::string keyLine = base + "_line";
  std::string keySignal = base + "_signal";
  std::string keyHist = base + "_hist";

  if (indicatorCache.find(keyLine) == indicatorCache.end()) {
    auto result = indicators::macd(buyPrices, fast, slow, signal);
    indicatorCache[keyLine] = std::move(result.macd_line);
    indicatorCache[keySignal] = std::move(result.signal_line);
    indicatorCache[keyHist] = std::move(result.histogram);
  }
  return {sliceResult(indicatorCache[keyLine], start, end),
          sliceResult(indicatorCache[keySignal], start, end),
          sliceResult(indicatorCache[keyHist], start, end)};
}
std::tuple<std::vector<float>, std::vector<float>, std::vector<float>>
MarketData::getSellMACD(int fast, int slow, int signal, int start, int end) {
  std::string base = "sell_macd_" + std::to_string(fast) + "_" +
                     std::to_string(slow) + "_" + std::to_string(signal);
  std::string keyLine = base + "_line";
  std::string keySignal = base + "_signal";
  std::string keyHist = base + "_hist";

  if (indicatorCache.find(keyLine) == indicatorCache.end()) {
    auto result = indicators::macd(sellPrices, fast, slow, signal);
    indicatorCache[keyLine] = std::move(result.macd_line);
    indicatorCache[keySignal] = std::move(result.signal_line);
    indicatorCache[keyHist] = std::move(result.histogram);
  }
  return {sliceResult(indicatorCache[keyLine], start, end),
          sliceResult(indicatorCache[keySignal], start, end),
          sliceResult(indicatorCache[keyHist], start, end)};
}

// Bollinger Bands
std::tuple<std::vector<float>, std::vector<float>, std::vector<float>>
MarketData::getBuyBollingerBands(int period, float std_dev, int start, int end) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%.2f", std_dev);
  std::string base = "buy_bb_" + std::to_string(period) + "_" + buf;
  std::string keyUpper = base + "_upper";
  std::string keyMiddle = base + "_middle";
  std::string keyLower = base + "_lower";

  if (indicatorCache.find(keyUpper) == indicatorCache.end()) {
    auto result = indicators::bollinger(buyPrices, period, std_dev);
    indicatorCache[keyUpper] = std::move(result.upper);
    indicatorCache[keyMiddle] = std::move(result.middle);
    indicatorCache[keyLower] = std::move(result.lower);
  }
  return {sliceResult(indicatorCache[keyUpper], start, end),
          sliceResult(indicatorCache[keyMiddle], start, end),
          sliceResult(indicatorCache[keyLower], start, end)};
}
std::tuple<std::vector<float>, std::vector<float>, std::vector<float>>
MarketData::getSellBollingerBands(int period, float std_dev, int start, int end) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%.2f", std_dev);
  std::string base = "sell_bb_" + std::to_string(period) + "_" + buf;
  std::string keyUpper = base + "_upper";
  std::string keyMiddle = base + "_middle";
  std::string keyLower = base + "_lower";

  if (indicatorCache.find(keyUpper) == indicatorCache.end()) {
    auto result = indicators::bollinger(sellPrices, period, std_dev);
    indicatorCache[keyUpper] = std::move(result.upper);
    indicatorCache[keyMiddle] = std::move(result.middle);
    indicatorCache[keyLower] = std::move(result.lower);
  }
  return {sliceResult(indicatorCache[keyUpper], start, end),
          sliceResult(indicatorCache[keyMiddle], start, end),
          sliceResult(indicatorCache[keyLower], start, end)};
}

// ATR
std::vector<float> MarketData::getATR(int period, int start, int end) {
  std::string key = "atr_" + std::to_string(period);
  if (indicatorCache.find(key) == indicatorCache.end()) {
    indicatorCache[key] = indicators::atr(buyPrices, sellPrices, period);
  }
  return sliceResult(indicatorCache[key], start, end);
}

// Mid SMA
std::vector<float> MarketData::getMidSMA(int period, int start, int end) {
  std::string key = "mid_sma_" + std::to_string(period);
  const auto& data = getCachedOrCompute(key, midPrices,
      [period](const std::vector<float>& p) { return indicators::sma(p, period); });
  return sliceResult(data, start, end);
}

// Mid EMA
std::vector<float> MarketData::getMidEMA(int period, int start, int end) {
  std::string key = "mid_ema_" + std::to_string(period);
  const auto& data = getCachedOrCompute(key, midPrices,
      [period](const std::vector<float>& p) { return indicators::ema(p, period); });
  return sliceResult(data, start, end);
}

// Mid RSI
std::vector<float> MarketData::getMidRSI(int period, int start, int end) {
  std::string key = "mid_rsi_" + std::to_string(period);
  const auto& data = getCachedOrCompute(key, midPrices,
      [period](const std::vector<float>& p) { return indicators::rsi(p, period); });
  return sliceResult(data, start, end);
}

// Mid MACD
std::tuple<std::vector<float>, std::vector<float>, std::vector<float>>
MarketData::getMidMACD(int fast, int slow, int signal, int start, int end) {
  std::string base = "mid_macd_" + std::to_string(fast) + "_" +
                     std::to_string(slow) + "_" + std::to_string(signal);
  std::string keyLine = base + "_line";
  std::string keySignal = base + "_signal";
  std::string keyHist = base + "_hist";

  if (indicatorCache.find(keyLine) == indicatorCache.end()) {
    auto result = indicators::macd(midPrices, fast, slow, signal);
    indicatorCache[keyLine] = std::move(result.macd_line);
    indicatorCache[keySignal] = std::move(result.signal_line);
    indicatorCache[keyHist] = std::move(result.histogram);
  }
  return {sliceResult(indicatorCache[keyLine], start, end),
          sliceResult(indicatorCache[keySignal], start, end),
          sliceResult(indicatorCache[keyHist], start, end)};
}

// Mid Bollinger Bands
std::tuple<std::vector<float>, std::vector<float>, std::vector<float>>
MarketData::getMidBollingerBands(int period, float std_dev, int start, int end) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%.2f", std_dev);
  std::string base = "mid_bb_" + std::to_string(period) + "_" + buf;
  std::string keyUpper = base + "_upper";
  std::string keyMiddle = base + "_middle";
  std::string keyLower = base + "_lower";

  if (indicatorCache.find(keyUpper) == indicatorCache.end()) {
    auto result = indicators::bollinger(midPrices, period, std_dev);
    indicatorCache[keyUpper] = std::move(result.upper);
    indicatorCache[keyMiddle] = std::move(result.middle);
    indicatorCache[keyLower] = std::move(result.lower);
  }
  return {sliceResult(indicatorCache[keyUpper], start, end),
          sliceResult(indicatorCache[keyMiddle], start, end),
          sliceResult(indicatorCache[keyLower], start, end)};
}
