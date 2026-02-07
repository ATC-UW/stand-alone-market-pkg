#pragma once
#include "Challenge.h"
#include <memory>
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

  MarketData(float startBuyPrice, float startSellPrice);

  void init();
  void updatePriceValue();
  float getBuyPrice(int day);
  float getSellPrice(int day);
  float getNextBuyPrice();
  float getNextSellPrice();
};
