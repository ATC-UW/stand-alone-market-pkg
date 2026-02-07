#include "Regime.h"

// --- RandomWalkRegime ---

RandomWalkRegime::RandomWalkRegime(float volatility) : volatility(volatility) {}

float RandomWalkRegime::update(float val, std::mt19937 &rng) {
  std::uniform_real_distribution<float> noiseDist(-val / 50.0f, val / 50.0f);
  float noise = noiseDist(rng);
  val += noise;
  float change = val * volatility;
  std::uniform_real_distribution<float> coinDist(0.0f, 1.0f);
  if (coinDist(rng) > 0.5f) {
    change = -change;
  }
  return val + change;
}

// --- SineWaveRegime ---

SineWaveRegime::SineWaveRegime(float volatility, float amplitude, float phase)
    : volatility(volatility), amplitude(amplitude), phase(phase), dayIndex(0) {}

void SineWaveRegime::setDayIndex(int day) { dayIndex = day; }

float SineWaveRegime::update(float val, std::mt19937 &rng) {
  std::uniform_real_distribution<float> noiseDist(-val * volatility,
                                                  val * volatility);
  float noise = noiseDist(rng);
  val += noise;
  float sineValue = amplitude * std::sin(static_cast<float>(dayIndex) + phase);
  return val + sineValue;
}

// --- DropRegime ---

DropRegime::DropRegime(float rate) : rate(rate) {}

float DropRegime::update(float val, std::mt19937 &rng) {
  std::uniform_real_distribution<float> noiseDist(-val * rate, val * rate);
  float noise = noiseDist(rng);
  val += noise;
  return val - val * rate;
}

// --- SpikeRegime ---

SpikeRegime::SpikeRegime(float rate) : rate(rate) {}

float SpikeRegime::update(float val, std::mt19937 &rng) {
  std::uniform_real_distribution<float> noiseDist(-val * rate, val * rate);
  float noise = noiseDist(rng);
  val += noise;
  return val + val * rate;
}

// --- GBMRegime ---

GBMRegime::GBMRegime(float mu, float sigma) : mu(mu), sigma(sigma) {}

float GBMRegime::update(float val, std::mt19937 &rng) {
  std::normal_distribution<float> norm(0.0f, 1.0f);
  float z = norm(rng);
  float dt = 1.0f;
  return val * std::exp((mu - 0.5f * sigma * sigma) * dt +
                        sigma * std::sqrt(dt) * z);
}

// --- MeanReversionRegime ---

MeanReversionRegime::MeanReversionRegime(float mu, float theta, float sigma)
    : mu(mu), theta(theta), sigma(sigma) {}

float MeanReversionRegime::update(float val, std::mt19937 &rng) {
  std::normal_distribution<float> norm(0.0f, 1.0f);
  float z = norm(rng);
  float dt = 1.0f;
  return val + theta * (mu - val) * dt + sigma * z;
}

// --- JumpDiffusionRegime ---

JumpDiffusionRegime::JumpDiffusionRegime(float mu, float sigma,
                                         float jumpIntensity, float jumpSize)
    : mu(mu), sigma(sigma), jumpIntensity(jumpIntensity),
      jumpSize(jumpSize) {}

float JumpDiffusionRegime::update(float val, std::mt19937 &rng) {
  // GBM component
  std::normal_distribution<float> norm(0.0f, 1.0f);
  float z = norm(rng);
  float dt = 1.0f;
  float gbmPrice =
      val * std::exp((mu - 0.5f * sigma * sigma) * dt +
                     sigma * std::sqrt(dt) * z);

  // Jump component
  std::uniform_real_distribution<float> uniformDist(0.0f, 1.0f);
  if (uniformDist(rng) < jumpIntensity) {
    std::normal_distribution<float> jumpDist(jumpSize, std::abs(jumpSize));
    float jump = jumpDist(rng);
    gbmPrice *= (1.0f + jump);
  }

  return gbmPrice;
}
