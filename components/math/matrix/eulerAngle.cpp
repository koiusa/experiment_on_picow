#include "eulerAngle.h"

float EulerAngle::toRadians(float degrees) {
  return degrees * (M_PI / 180.0f);
};

float EulerAngle::toDegrees(float radians) {
  return radians * (180.0f / M_PI);
};

float EulerAngle::RepeatAngle(float radians) {
  float result = std::fmod(radians, M_PI * 2);
  if(result < 0) result += M_PI * 2;
  return result;
};

float EulerAngle::WrapAngle(float radians) {
  float result = std::fmod(radians, M_PI * 2);
  if(result < 0) result += M_PI * 2;
  result -= M_PI;
  return result;
};
