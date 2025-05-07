#ifndef ROTATIONMATRIX_H
#define ROTATIONMATRIX_H

#include <cmath>
#include <array>

// Definition of Vector3 class
class Vector3 {
public:
  float x, y, z;
  Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};

class RotationMatrix {
public:
  std::array<float, 9> elements;
  RotationMatrix(std::array<float, 9> elements): elements(elements) {}
  RotationMatrix operator*(const RotationMatrix m) const;
  Vector3 operator*(const Vector3 v) const;
  float& operator[](const size_t index);
  float& at(const size_t row, const size_t column);
};

#endif // ROTATIONMATRIX_H
