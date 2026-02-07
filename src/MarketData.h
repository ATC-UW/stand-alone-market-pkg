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

  float getNextBuyPrice();
  float getNextSellPrice();

private:
  std::vector<std::shared_ptr<Regime>> dayRegimes;
  std::vector<float> buyPrices;
  std::vector<float> sellPrices;
  int currentDay;
  int totalDays;
  bool getNextBuyPriceCalled;
  bool getNextSellPriceCalled;
  std::mt19937 rng;

  void computePrices();
  float getBuyPrice(int day);
  float getSellPrice(int day);
};
