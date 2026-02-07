#pragma once
#include "utils.h"
#include <cmath>
#include <functional>
#include <memory>
#include <unordered_map>

struct ChallengeConfig {
  int challengeType;
  float volatility;
  int day;

  ChallengeConfig(int type, float vol, int d)
      : challengeType(type), volatility(vol), day(d) {}

  ChallengeConfig() : challengeType(0), volatility(0), day(0) {}
};

class Challenge {
public:
  Challenge(ChallengeConfig &config) {};
  virtual float update(float val) = 0;
};

class Challenge0 : public Challenge {
public:
  Challenge0(ChallengeConfig &config) : Challenge(config) {}
  float update(float val) override { return val; }
};

class Challenge1 : public Challenge {
private:
  float volatility;

public:
  Challenge1(ChallengeConfig &config)
      : Challenge(config), volatility(config.volatility) {};
  float update(float val) override { return val * volatility; }
};

class Challenge2 : public Challenge {
private:
  float volatility;

public:
  Challenge2(ChallengeConfig &config)
      : Challenge(config), volatility(config.volatility) {};
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

class Challenge3 : public Challenge {
private:
  float vol;
  float amplitude;
  float phase;
  int day;

public:
  Challenge3(ChallengeConfig &config)
      : Challenge(config), vol(config.volatility), amplitude(1.0), phase(0.0),
        day(config.day) {}

  float update(float val) override {
    float noise = utils::random(-val * vol, val * vol);
    val += noise;
    float sineValue = amplitude * std::sin(day);
    return val + sineValue;
  }
};

class Challenge4 : public Challenge {
private:
  float dropRate;

public:
  Challenge4(ChallengeConfig &config)
      : Challenge(config), dropRate(config.volatility) {}

  float update(float val) override {
    float noise = utils::random(-val * dropRate, val * dropRate);
    val += noise;
    return val - val * dropRate;
  }
};

class Challenge5 : public Challenge {
private:
  float upRate;

public:
  Challenge5(ChallengeConfig &config)
      : Challenge(config), upRate(config.volatility) {}

  float update(float val) override {
    float noise = utils::random(-val * upRate, val * upRate);
    val += noise;
    return val + val * upRate;
  }
};

using ChallengeCreator =
    std::function<std::shared_ptr<Challenge>(ChallengeConfig &)>;

class ChallengeFactory {
private:
  static std::unordered_map<int, ChallengeCreator> challengeMap;

public:
  static void registerChallenge(int type, ChallengeCreator creator);
  static std::shared_ptr<Challenge> createChallenge(ChallengeConfig &config);
  static void registerChallenges();
};
