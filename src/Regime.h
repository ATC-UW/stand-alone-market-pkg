#pragma once
#include <cmath>
#include <memory>
#include <random>

class Regime {
public:
  virtual ~Regime() = default;
  virtual void setDayIndex(int day) { (void)day; }
  virtual float update(float val, std::mt19937 &rng) = 0;
};

class RandomWalkRegime : public Regime {
private:
  float volatility;

public:
  explicit RandomWalkRegime(float volatility);
  float update(float val, std::mt19937 &rng) override;
};

class SineWaveRegime : public Regime {
private:
  float volatility;
  float amplitude;
  float phase;
  int dayIndex;

public:
  SineWaveRegime(float volatility, float amplitude, float phase);
  void setDayIndex(int day) override;
  float update(float val, std::mt19937 &rng) override;
};

class DropRegime : public Regime {
private:
  float rate;

public:
  explicit DropRegime(float rate);
  float update(float val, std::mt19937 &rng) override;
};

class SpikeRegime : public Regime {
private:
  float rate;

public:
  explicit SpikeRegime(float rate);
  float update(float val, std::mt19937 &rng) override;
};

class GBMRegime : public Regime {
private:
  float mu;
  float sigma;

public:
  GBMRegime(float mu, float sigma);
  float update(float val, std::mt19937 &rng) override;
};

class MeanReversionRegime : public Regime {
private:
  float mu;
  float theta;
  float sigma;

public:
  MeanReversionRegime(float mu, float theta, float sigma);
  float update(float val, std::mt19937 &rng) override;
};

class JumpDiffusionRegime : public Regime {
private:
  float mu;
  float sigma;
  float jumpIntensity;
  float jumpSize;

public:
  JumpDiffusionRegime(float mu, float sigma, float jumpIntensity,
                      float jumpSize);
  float update(float val, std::mt19937 &rng) override;
};

class MomentumRegime : public Regime {
private:
  float mu;
  float sigma;
  float momentum;
  float prevReturn;

public:
  MomentumRegime(float mu, float sigma, float momentum);
  float update(float val, std::mt19937 &rng) override;
};

class TrendingMeanReversionRegime : public Regime {
private:
  float mu;
  float drift;
  float theta;
  float sigma;
  int step;

public:
  TrendingMeanReversionRegime(float mu, float drift, float theta, float sigma);
  float update(float val, std::mt19937 &rng) override;
};

struct RegimeAssignment {
  std::shared_ptr<Regime> regime;
  int startDay;
  int endDay;

  RegimeAssignment(std::shared_ptr<Regime> regime, int startDay, int endDay)
      : regime(std::move(regime)), startDay(startDay), endDay(endDay) {}
};
