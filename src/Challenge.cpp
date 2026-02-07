#include "Challenge.h"

std::unordered_map<int, ChallengeCreator> ChallengeFactory::challengeMap;

void ChallengeFactory::registerChallenge(int type, ChallengeCreator creator) {
  challengeMap[type] = creator;
}

std::shared_ptr<Challenge> ChallengeFactory::createChallenge(ChallengeConfig &config) {
  if (challengeMap.find(config.challengeType) == challengeMap.end()) {
    return std::make_shared<Challenge0>(config);
  }

  return challengeMap[config.challengeType](config);
}

void ChallengeFactory::registerChallenges() {
  registerChallenge(0, [=](ChallengeConfig &config) {
    return std::make_shared<Challenge0>(config);
  });

  registerChallenge(1, [=](ChallengeConfig &config) {
    return std::make_shared<Challenge1>(config);
  });

  registerChallenge(2, [=](ChallengeConfig &config) {
    return std::make_shared<Challenge2>(config);
  });

  registerChallenge(3, [=](ChallengeConfig &config) {
    return std::make_shared<Challenge3>(config);
  });

  registerChallenge(4, [=](ChallengeConfig &config) {
    return std::make_shared<Challenge4>(config);
  });

  registerChallenge(5, [=](ChallengeConfig &config) {
    return std::make_shared<Challenge5>(config);
  });
}
