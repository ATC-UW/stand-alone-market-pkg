#include "MarketData.h"
#include <algorithm>
#include <chrono>
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
