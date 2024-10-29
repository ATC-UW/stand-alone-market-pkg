#pragma once
#include "utils.h"
#include <memory>

class Challenge {
public:
  virtual float update(float val) = 0;
};

class Challenge0 : public Challenge {
public:
  float update(float val) override { return val; }
};

class Challenge1 : public Challenge {
private:
  float volatility;

public:
  Challenge1(float vol) : volatility(vol) {};
  float update(float val) override { return val * volatility; }
};

class Challenge2 : public Challenge {
private:
  float volatility;

public:
  Challenge2(float vol) : volatility(vol) {};
  float update(float val) override {
    // generate some noise that is within 10% of val
    float noise = utils::random(-val / 50, val / 50);
    val += noise;
    float change = val * volatility;
    // // Randomly change the sign of the change
    if (utils::random(0, 1) > 0.5) {
      change = -change;
    }

    return val + change;
  }
};

class ChallengeFactory {
public:
  static std::shared_ptr<Challenge> createChallenge(int challengeType,
                                                    float volatility = 100) {
    switch (challengeType) {
    case 1:
      return std::make_shared<Challenge1>(volatility);
    case 2:
      return std::make_shared<Challenge2>(volatility);
    default:
      return std::make_shared<Challenge0>();
    }
  }
};
