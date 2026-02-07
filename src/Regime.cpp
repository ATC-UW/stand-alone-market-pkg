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

// --- MomentumRegime ---

MomentumRegime::MomentumRegime(float mu, float sigma, float momentum)
    : mu(mu), sigma(sigma), momentum(momentum), prevReturn(0.0f) {}

float MomentumRegime::update(float val, std::mt19937 &rng) {
  std::normal_distribution<float> norm(0.0f, 1.0f);
  float z = norm(rng);
  float dt = 1.0f;
  float driftEff = mu + momentum * prevReturn;
  float newVal = val * std::exp((driftEff - 0.5f * sigma * sigma) * dt +
                                 sigma * std::sqrt(dt) * z);
  prevReturn = (newVal - val) / val;
  return newVal;
}

// --- TrendingMeanReversionRegime ---

TrendingMeanReversionRegime::TrendingMeanReversionRegime(float mu, float drift,
                                                         float theta, float sigma)
    : mu(mu), drift(drift), theta(theta), sigma(sigma), step(0) {}

float TrendingMeanReversionRegime::update(float val, std::mt19937 &rng) {
  std::normal_distribution<float> norm(0.0f, 1.0f);
  float z = norm(rng);
  float dt = 1.0f;
  float trendingMu = mu + drift * static_cast<float>(step);
  float newVal = val + theta * (trendingMu - val) * dt + sigma * z;
  step++;
  return newVal;
}

// --- EarningsRegime ---

EarningsRegime::EarningsRegime(float targetMin, float targetMax, int numDays,
                               float noise)
    : targetMin(targetMin), targetMax(targetMax), numDays(numDays), noise(noise),
      startDay(-1), relativeDay(0), targetPrice(0.0f), basePrice(0.0f),
      noiseAccum(0.0f), mode(-1), initialized(false) {}

void EarningsRegime::setDayIndex(int day) {
  if (startDay < 0)
    startDay = day;
  relativeDay = day - startDay;
}

float EarningsRegime::update(float val, std::mt19937 &rng) {
  if (!initialized) {
    basePrice = val;
    std::uniform_int_distribution<int> modeDist(0, 2);
    mode = modeDist(rng);
    std::uniform_real_distribution<float> targetDist(targetMin, targetMax);
    targetPrice = targetDist(rng);
    initialized = true;
  }

  float progress =
      (numDays <= 1) ? 1.0f
                     : static_cast<float>(relativeDay) / static_cast<float>(numDays - 1);
  progress = std::min(progress, 1.0f);

  float price;
  if (mode == 0) {
    // Instant jump: snap to target on first day
    price = (relativeDay == 0)
                ? basePrice
                : targetPrice;
  } else if (mode == 1) {
    // Linear ramp
    price = basePrice + (targetPrice - basePrice) * progress;
  } else {
    // Ease-in-out (smoothstep)
    float t = progress * progress * (3.0f - 2.0f * progress);
    price = basePrice + (targetPrice - basePrice) * t;
  }

  // Mean-reverting GBM-style noise
  std::normal_distribution<float> norm(0.0f, 1.0f);
  float z = norm(rng);
  noiseAccum = noiseAccum * 0.95f + noise * z;
  return price * (1.0f + noiseAccum);
}

// --- DeadCatBounceRegime ---

DeadCatBounceRegime::DeadCatBounceRegime(float dropRate, float recoveryRate,
                                         float declineRate, int numDays,
                                         float noise)
    : dropRate(dropRate), recoveryRate(recoveryRate), declineRate(declineRate),
      numDays(numDays), noise(noise), startDay(-1), relativeDay(0),
      basePrice(0.0f), noiseAccum(0.0f), initialized(false) {}

void DeadCatBounceRegime::setDayIndex(int day) {
  if (startDay < 0)
    startDay = day;
  relativeDay = day - startDay;
}

float DeadCatBounceRegime::update(float val, std::mt19937 &rng) {
  if (!initialized) {
    basePrice = val;
    initialized = true;
  }

  int phase1End = numDays * 30 / 100;
  int phase2End = numDays * 60 / 100;
  if (phase1End < 1) phase1End = 1;
  if (phase2End <= phase1End) phase2End = phase1End + 1;

  float dropBottom = basePrice * (1.0f - dropRate);
  float bounceTop = dropBottom + (basePrice - dropBottom) * recoveryRate;
  float finalPrice = bounceTop * (1.0f - declineRate);

  float price;
  if (relativeDay < phase1End) {
    // Phase 1: Drop
    float t = static_cast<float>(relativeDay) / static_cast<float>(phase1End);
    t = t * t * (3.0f - 2.0f * t); // smoothstep
    price = basePrice + (dropBottom - basePrice) * t;
  } else if (relativeDay < phase2End) {
    // Phase 2: Recovery
    float t = static_cast<float>(relativeDay - phase1End) /
              static_cast<float>(phase2End - phase1End);
    t = t * t * (3.0f - 2.0f * t); // smoothstep
    price = dropBottom + (bounceTop - dropBottom) * t;
  } else {
    // Phase 3: Decline
    int phase3Len = numDays - phase2End;
    float t = (phase3Len <= 0)
                  ? 1.0f
                  : static_cast<float>(relativeDay - phase2End) /
                        static_cast<float>(phase3Len);
    t = std::min(t, 1.0f);
    t = t * t * (3.0f - 2.0f * t); // smoothstep
    price = bounceTop + (finalPrice - bounceTop) * t;
  }

  // Mean-reverting GBM-style noise
  std::normal_distribution<float> norm(0.0f, 1.0f);
  float z = norm(rng);
  noiseAccum = noiseAccum * 0.95f + noise * z;
  return price * (1.0f + noiseAccum);
}

// --- InverseDeadCatBounceRegime ---

InverseDeadCatBounceRegime::InverseDeadCatBounceRegime(
    float riseRate, float pullbackRate, float continueRate, int numDays,
    float noise)
    : riseRate(riseRate), pullbackRate(pullbackRate),
      continueRate(continueRate), numDays(numDays), noise(noise),
      startDay(-1), relativeDay(0), basePrice(0.0f), noiseAccum(0.0f),
      initialized(false) {}

void InverseDeadCatBounceRegime::setDayIndex(int day) {
  if (startDay < 0)
    startDay = day;
  relativeDay = day - startDay;
}

float InverseDeadCatBounceRegime::update(float val, std::mt19937 &rng) {
  if (!initialized) {
    basePrice = val;
    initialized = true;
  }

  int phase1End = numDays * 30 / 100;
  int phase2End = numDays * 60 / 100;
  if (phase1End < 1) phase1End = 1;
  if (phase2End <= phase1End) phase2End = phase1End + 1;

  float riseTop = basePrice * (1.0f + riseRate);
  float pullbackBottom = riseTop - (riseTop - basePrice) * pullbackRate;
  float finalPrice = pullbackBottom * (1.0f + continueRate);

  float price;
  if (relativeDay < phase1End) {
    // Phase 1: Rise
    float t = static_cast<float>(relativeDay) / static_cast<float>(phase1End);
    t = t * t * (3.0f - 2.0f * t); // smoothstep
    price = basePrice + (riseTop - basePrice) * t;
  } else if (relativeDay < phase2End) {
    // Phase 2: Pullback
    float t = static_cast<float>(relativeDay - phase1End) /
              static_cast<float>(phase2End - phase1End);
    t = t * t * (3.0f - 2.0f * t); // smoothstep
    price = riseTop + (pullbackBottom - riseTop) * t;
  } else {
    // Phase 3: Continue rise
    int phase3Len = numDays - phase2End;
    float t = (phase3Len <= 0)
                  ? 1.0f
                  : static_cast<float>(relativeDay - phase2End) /
                        static_cast<float>(phase3Len);
    t = std::min(t, 1.0f);
    t = t * t * (3.0f - 2.0f * t); // smoothstep
    price = pullbackBottom + (finalPrice - pullbackBottom) * t;
  }

  // Mean-reverting GBM-style noise
  std::normal_distribution<float> norm(0.0f, 1.0f);
  float z = norm(rng);
  noiseAccum = noiseAccum * 0.95f + noise * z;
  return price * (1.0f + noiseAccum);
}
