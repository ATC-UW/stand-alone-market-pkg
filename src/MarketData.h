#pragma once
#include "Challenge.h"
#include <ctime>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <vector>

const int numberOfChallenges = 60;

class MarketData {
public:
  std::vector<std::shared_ptr<Challenge>> challenges;
  std::vector<float> buyPrices;
  std::vector<float> sellPrices;
  int currentDay;
  bool getNextBuyPriceCalled = false;
  bool getNextSellPriceCalled = false;

  MarketData(float startBuyPrice, float startSellPrice) {
    srand(time(NULL));
    for (int i = 0; i < numberOfChallenges; i++) {
      challenges.push_back(ChallengeFactory::createChallenge(0));
    }

    for (int i = 0; i < numberOfChallenges; i += 10) {
      challenges[i] = ChallengeFactory::createChallenge(2, 0.01);
    }

    buyPrices.push_back(startBuyPrice);
    sellPrices.push_back(startSellPrice);
    currentDay = 0;
    this->init();
  }

  void init() {
    currentDay = 0;
    updatePriceValue();
  }

  void updatePriceValue() {
    for (int i = 0; i < numberOfChallenges; i++) {
      buyPrices.push_back(challenges[i]->update(buyPrices.back()));
      sellPrices.push_back(challenges[i]->update(sellPrices.back()));
    }
  }

  float getBuyPrice(int day) {
    if (day < 0 || day >= buyPrices.size()) {
      return -1;
    }
    return buyPrices[day];
  }

  float getSellPrice(int day) {
    if (day < 0 || day >= sellPrices.size()) {
      return -1;
    }
    return sellPrices[day];
  }

  float getNextBuyPrice() {
    float ret = getBuyPrice(currentDay);
    getNextBuyPriceCalled = true;

    if (getNextSellPriceCalled) {
      currentDay++;
      getNextBuyPriceCalled = false;
      getNextSellPriceCalled = false;
    }
    return ret;
  }
  float getNextSellPrice() {
    float ret = getSellPrice(currentDay);
    getNextSellPriceCalled = true;
    if (getNextBuyPriceCalled) {
      currentDay++;
      getNextBuyPriceCalled = false;
      getNextSellPriceCalled = false;
    }
    return ret;
  }
};