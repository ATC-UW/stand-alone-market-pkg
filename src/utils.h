#pragma once
#include <stdlib.h>

namespace utils {
float random(float a, float b) {
  float random = ((float)rand()) / (float)RAND_MAX;
  float diff = b - a;
  float r = random * diff;
  return a + r;
}
} // namespace utils
