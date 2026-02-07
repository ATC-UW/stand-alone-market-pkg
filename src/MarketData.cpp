#include "MarketData.h"
#include <algorithm>
#include <chrono>

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
  currentDay = 0;
  getNextBuyPriceCalled = false;
  getNextSellPriceCalled = false;

  computePrices();
}

void MarketData::computePrices() {
  for (int i = 0; i < totalDays; i++) {
    if (dayRegimes[i]) {
      dayRegimes[i]->setDayIndex(i);
      buyPrices.push_back(dayRegimes[i]->update(buyPrices.back(), rng));
      sellPrices.push_back(dayRegimes[i]->update(sellPrices.back(), rng));
    } else {
      buyPrices.push_back(buyPrices.back());
      sellPrices.push_back(sellPrices.back());
    }
  }
}

float MarketData::getBuyPrice(int day) {
  if (day < 0 || day >= static_cast<int>(buyPrices.size())) {
    return -1;
  }
  return buyPrices[day];
}

float MarketData::getSellPrice(int day) {
  if (day < 0 || day >= static_cast<int>(sellPrices.size())) {
    return -1;
  }
  return sellPrices[day];
}

float MarketData::getNextBuyPrice() {
  float ret = getBuyPrice(currentDay);
  getNextBuyPriceCalled = true;

  if (getNextSellPriceCalled) {
    currentDay++;
    getNextBuyPriceCalled = false;
    getNextSellPriceCalled = false;
  }
  return ret;
}

float MarketData::getNextSellPrice() {
  float ret = getSellPrice(currentDay);
  getNextSellPriceCalled = true;
  if (getNextBuyPriceCalled) {
    currentDay++;
    getNextBuyPriceCalled = false;
    getNextSellPriceCalled = false;
  }
  return ret;
}
