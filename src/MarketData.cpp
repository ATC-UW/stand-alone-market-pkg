#include "MarketData.h"
#include <cstdlib>
#include <ctime>

MarketData::MarketData(float startBuyPrice, float startSellPrice) {
  ChallengeFactory::registerChallenges();
  srand(time(NULL));

  for (int i = 0; i < numberOfChallenges; i++) {
    ChallengeConfig config(3, 0.01, i);
    challenges.push_back(ChallengeFactory::createChallenge(config));
  }

  for (int i = 10; i < numberOfChallenges; i += 2) {
    ChallengeConfig config(4, 0.01, i);
    challenges[i] = ChallengeFactory::createChallenge(config);
  }

  for (int i = 0; i < numberOfChallenges - 30; i += 8) {
    ChallengeConfig config(5, 0.05, i);
    challenges[i] = ChallengeFactory::createChallenge(config);
  }

  for (int i = 0; i < numberOfChallenges; i += 7) {
    ChallengeConfig config(2, 0.01, i);
    challenges[i] = ChallengeFactory::createChallenge(config);
  }

  buyPrices.push_back(startBuyPrice);
  sellPrices.push_back(startSellPrice);
  currentDay = 0;
  this->init();
}

void MarketData::init() {
  currentDay = 0;
  updatePriceValue();
}

void MarketData::updatePriceValue() {
  for (int i = 0; i < numberOfChallenges; i++) {
    buyPrices.push_back(challenges[i]->update(buyPrices.back()));
    sellPrices.push_back(challenges[i]->update(sellPrices.back()));
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
