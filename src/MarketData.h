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
    ChallengeFactory::registerChallenges();
    srand(time(NULL));
    // for (int i = 0; i < numberOfChallenges; i++) {
    //   ChallengeConfig config(0, 0.0f, i);
    //   challenges.push_back(ChallengeFactory::createChallenge(config));
    // }

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