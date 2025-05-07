#ifndef EULERANGLE_H
#define EULERANGLE_H

#include <cmath>

enum class EulerOrder {
  XYZ,
  XZY,
  YXZ,
  YZX,
  ZXY,
  ZYX
};

class EulerAngle {
public:
  float x;
  float y;
  float z;
  EulerOrder order;
  EulerAngle(float x, float y, float z, EulerOrder order): x(x), y(y), z(z), order(order) {}
  void setvalues(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
  EulerAngle toRadians() const {
    return EulerAngle(toRadians(x), toRadians(y), toRadians(z), order);
  }
  static float toRadians(float degrees);
  static float toDegrees(float radians);
  static float RepeatAngle(float angle);
  static float WrapAngle(float angle);
};

#endif // EULERANGLE_H
