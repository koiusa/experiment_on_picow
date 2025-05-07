#include "rotationMatrix.h"

RotationMatrix RotationMatrix::operator*(const RotationMatrix m) const {
  return RotationMatrix({
    elements[0] * m.elements[0] + elements[3] * m.elements[1] + elements[6] * m.elements[2],
    elements[1] * m.elements[0] + elements[4] * m.elements[1] + elements[7] * m.elements[2],
    elements[2] * m.elements[0] + elements[5] * m.elements[1] + elements[8] * m.elements[2],
    elements[0] * m.elements[3] + elements[3] * m.elements[4] + elements[6] * m.elements[5],
    elements[1] * m.elements[3] + elements[4] * m.elements[4] + elements[7] * m.elements[5],
    elements[2] * m.elements[3] + elements[5] * m.elements[4] + elements[8] * m.elements[5],
    elements[0] * m.elements[6] + elements[3] * m.elements[7] + elements[6] * m.elements[8],
    elements[1] * m.elements[6] + elements[4] * m.elements[7] + elements[7] * m.elements[8],
    elements[2] * m.elements[6] + elements[5] * m.elements[7] + elements[8] * m.elements[8]
  });
};

Vector3 RotationMatrix::operator*(const Vector3 v) const {
  return Vector3(
    elements[0] * v.x + elements[3] * v.y + elements[6] * v.z,
    elements[1] * v.x + elements[4] * v.y + elements[7] * v.z,
    elements[2] * v.x + elements[5] * v.y + elements[8] * v.z
  );
};

float& RotationMatrix::operator[](const size_t index) {
  return elements[index];
};

float& RotationMatrix::at(const size_t row, const size_t column) {
  return elements[row + column * 3];
};
