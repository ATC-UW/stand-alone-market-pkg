#pragma once
#include "Indicator.h"
#include "Regime.h"
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <tuple>
#include <vector>

class MarketData {
public:
  MarketData(float startBuyPrice, float startSellPrice,
             std::vector<RegimeAssignment> regimes,
             std::optional<unsigned int> seed = std::nullopt);

  std::vector<float> getBuyPrices(int start = 0, int end = -1);
  std::vector<float> getSellPrices(int start = 0, int end = -1);
  int getTotalDays();

  // Technical indicators - Buy
  std::vector<float> getBuySMA(int period = 20, int start = 0, int end = -1);
  std::vector<float> getBuyEMA(int period = 20, int start = 0, int end = -1);
  std::vector<float> getBuyRSI(int period = 14, int start = 0, int end = -1);
  std::tuple<std::vector<float>, std::vector<float>, std::vector<float>>
      getBuyMACD(int fast = 12, int slow = 26, int signal = 9,
                 int start = 0, int end = -1);
  std::tuple<std::vector<float>, std::vector<float>, std::vector<float>>
      getBuyBollingerBands(int period = 20, float std_dev = 2.0f,
                           int start = 0, int end = -1);

  // Technical indicators - Sell
  std::vector<float> getSellSMA(int period = 20, int start = 0, int end = -1);
  std::vector<float> getSellEMA(int period = 20, int start = 0, int end = -1);
  std::vector<float> getSellRSI(int period = 14, int start = 0, int end = -1);
  std::tuple<std::vector<float>, std::vector<float>, std::vector<float>>
      getSellMACD(int fast = 12, int slow = 26, int signal = 9,
                  int start = 0, int end = -1);
  std::tuple<std::vector<float>, std::vector<float>, std::vector<float>>
      getSellBollingerBands(int period = 20, float std_dev = 2.0f,
                            int start = 0, int end = -1);

  // ATR uses both price series
  std::vector<float> getATR(int period = 14, int start = 0, int end = -1);

private:
  std::vector<std::shared_ptr<Regime>> dayRegimes;
  std::vector<float> buyPrices;
  std::vector<float> sellPrices;
  int totalDays;
  std::mt19937 rng;

  void computePrices();

  // Indicator cache
  std::map<std::string, std::vector<float>> indicatorCache;

  std::vector<float> sliceResult(const std::vector<float>& data, int start, int end);
  const std::vector<float>& getCachedOrCompute(
      const std::string& key,
      const std::vector<float>& prices,
      std::function<std::vector<float>(const std::vector<float>&)> computeFn);
};
