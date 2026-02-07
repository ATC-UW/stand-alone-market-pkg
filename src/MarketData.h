#pragma once
#include "Regime.h"
#include <memory>
#include <optional>
#include <random>
#include <vector>

class MarketData {
public:
  MarketData(float startBuyPrice, float startSellPrice,
             std::vector<RegimeAssignment> regimes,
             std::optional<unsigned int> seed = std::nullopt);

  std::vector<float> getBuyPrices(int start = 0, int end = -1);
  std::vector<float> getSellPrices(int start = 0, int end = -1);
  int getTotalDays();

private:
  std::vector<std::shared_ptr<Regime>> dayRegimes;
  std::vector<float> buyPrices;
  std::vector<float> sellPrices;
  int totalDays;
  std::mt19937 rng;

  void computePrices();
};
