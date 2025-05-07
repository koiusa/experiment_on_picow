#ifndef QUATERNION_H
#define QUATERNION_H

#include "eulerAngle.h"
#include "rotationMatrix.h"
#include <cmath>

class Quaternion {
public:
  float x;
  float y;
  float z;
  float w;
  Quaternion() : x(0), y(0), z(0), w(1) {}
  Quaternion(float x, float y, float z, float w): x(x), y(y),z(z), w(w) {}
  Quaternion operator*(const Quaternion q) const;
  Vector3 rotate(const Vector3 v) const;
  static Quaternion conjugate(const Quaternion q);
  static Quaternion toQuaternion(EulerAngle e);
  static Quaternion toQuaternion(RotationMatrix m);
  static EulerAngle toEulerAngle(Quaternion q, EulerOrder order);
  static EulerAngle toEulerAngle(RotationMatrix m, EulerOrder order);
  static RotationMatrix toRotationMatrix(EulerAngle e);
  static RotationMatrix toRotationMatrix(Quaternion q);
};

#endif // QUATERNION_H
