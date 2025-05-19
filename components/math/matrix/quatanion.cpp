#include "quaternion.h"

Quaternion Quaternion::conjugate(const Quaternion q) {
  return Quaternion(-q.x, -q.y, -q.z, q.w);
};

Quaternion Quaternion::operator*(const Quaternion q) const {
  return Quaternion(
    w * q.x - z * q.y + y * q.z + x * q.w,
    z * q.x + w * q.y - x * q.z + y * q.w,
    -y * q.x + x * q.y + w * q.z + z * q.w,
    -x * q.x - y * q.y - z * q.z + w * q.w
  );
};

Vector3 Quaternion::rotate(const Vector3 v) const {
  auto vq = Quaternion(v.x, v.y, v.z, 0);
  auto cq = conjugate(*this);
  auto mq = *this * vq * cq;
  return Vector3(mq.x, mq.y, mq.z);
};

// Convert between Euler angles, rotation matrices, and quaternions
RotationMatrix Quaternion::toRotationMatrix(EulerAngle e) {
  auto cx = std::cos(e.x);
  auto sx = std::sin(e.x);
  auto cy = std::cos(e.y);
  auto sy = std::sin(e.y);
  auto cz = std::cos(e.z);
  auto sz = std::sin(e.z);
  switch (e.order) {
  case EulerOrder::XYZ:
    return RotationMatrix({
      cy * cz, sx * sy * cz + cx * sz, -cx * sy * cz + sx * sz,
      -cy * sz, -sx * sy * sz + cx * cz, cx * sy * sz + sx * cz,
      sy, -sx * cy, cx * cy
    });
  case EulerOrder::XZY:
    return RotationMatrix({
      cy * cz, cx * cy * sz + sx * sy, sx * cy * sz - cx * sy,
      -sz, cx * cz, sx * cz,
      sy * cz, cx * sy * sz - sx * cy, sx * sy * sz + cx * cy
    });
  case EulerOrder::YXZ:
    return RotationMatrix({
      sx * sy * sz + cy * cz, cx * sz, sx * cy * sz - sy * cz,
      sx * sy * cz - cy * sz, cx * cz, sx * cy * cz + sy * sz,
      cx * sy, -sx, cx * cy
    });
  case EulerOrder::YZX:
    return RotationMatrix({
      cy * cz, sz, -sy * cz,
      -cx * cy * sz + sx * sy, cx * cz, cx * sy * sz + sx * cy,
      sx * cy * sz + cx * sy, -sx * cz, -sx * sy * sz + cx * cy
    });
  case EulerOrder::ZXY:
    return RotationMatrix({
      -sx * sy * sz + cy * cz, sx * sy * cz + cy * sz, -cx * sy,
      -cx * sz, cx * cz, sx,
      sx * cy * sz + sy * cz, -sx * cy * cz + sy * sz, cx * cy
    });
  case EulerOrder::ZYX:
    return RotationMatrix({
      cy * cz, cy * sz, -sy,
      sx * sy * cz - cx * sz, sx * sy * sz + cx * cz, sx * cy,
      cx * sy * cz + sx * sz, cx * sy * sz - sx * cz, cx * cy
    });
  }
  throw "conversion of euler angle to rotation matrix is failed.";
};

// Convert between rotation matrices and Euler angles
EulerAngle Quaternion::toEulerAngle(RotationMatrix m, EulerOrder order) {
  if (order == EulerOrder::XYZ) {
    auto sy = m.at(0, 2);
    auto unlocked = std::abs(sy) < 0.99999f; 
    return EulerAngle(
      unlocked ? std::atan2(-m.at(1, 2), m.at(2, 2)) : std::atan2(m.at(2, 1), m.at(1, 1)),
      std::asin(sy),
      unlocked ? std::atan2(-m.at(0, 1), m.at(0, 0)) : 0,
      order
    );
  } else if (order == EulerOrder::XZY) {
    auto sz = -m.at(0, 1);
    auto unlocked = std::abs(sz) < 0.99999f;
    return EulerAngle(
      unlocked ? std::atan2(m.at(2, 1), m.at(1, 1)) : std::atan2(-m.at(1, 2), m.at(2, 2)),
      unlocked ? std::atan2(m.at(0, 2), m.at(0, 0)) : 0,
      std::asin(sz),
      order
    );
  } else if (order == EulerOrder::YXZ) {
    auto sx = -m.at(1, 2);
    auto unlocked = std::abs(sx) < 0.99999f;
    return EulerAngle(
      std::asin(sx),
      unlocked ? std::atan2(m.at(0, 2), m.at(2, 2)) : std::atan2(-m.at(2, 0), m.at(0, 0)),
      unlocked ? std::atan2(m.at(1, 0), m.at(1, 1)) : 0,
      order
    );
  } else if (order == EulerOrder::YZX) {
    auto sz = m.at(1, 0);
    auto unlocked = std::abs(sz) < 0.99999f;
    return EulerAngle(
      unlocked ? atan2(-m.at(1, 2), m.at(1, 1)) : 0,
      unlocked ? atan2(-m.at(2, 0), m.at(0, 0)) : atan2(m.at(0, 2), m.at(2, 2)),
      std::asin(sz),
      order
    );
  } else if (order == EulerOrder::ZXY) {
    auto sx = m.at(2, 1);
    auto unlocked = std::abs(sx) < 0.99999f;
    return EulerAngle(
      std::asin(sx),
      unlocked ? std::atan2(-m.at(2, 0), m.at(2, 2)) : 0,
      unlocked ? std::atan2(-m.at(0, 1), m.at(1, 1)) : std::atan2(m.at(1, 0), m.at(0, 0)),
      order
    );
  } else if (order == EulerOrder::ZYX) {
    auto sy = -m.at(2, 0);
    auto unlocked = std::abs(sy) < 0.99999f;
    return EulerAngle(
      unlocked ? std::atan2(m.at(2, 1), m.at(2, 2)) : 0,
      std::asin(sy),
      unlocked ? std::atan2(m.at(1, 0), m.at(0, 0)) : std::atan2(-m.at(0, 1), m.at(1, 1)),
      order
    );
  }
  throw "conversion of rotation matrix to euler angle is failed.";
};

// Convert between Euler angles and quaternions
Quaternion Quaternion::toQuaternion(EulerAngle e) {
  auto cx = std::cos(0.5f * e.x);
  auto sx = std::sin(0.5f * e.x);
  auto cy = std::cos(0.5f * e.y);
  auto sy = std::sin(0.5f * e.y);
  auto cz = std::cos(0.5f * e.z);
  auto sz = std::sin(0.5f * e.z);
  switch (e.order) {
  case EulerOrder::XYZ:
    return Quaternion(
      cx * sy * sz + sx * cy * cz,
      -sx * cy * sz + cx * sy * cz,
      cx * cy * sz + sx * sy * cz,
      -sx * sy * sz + cx * cy * cz
    );
  case EulerOrder::XZY:
    return Quaternion(
      -cx * sy * sz + sx * cy * cz,
      cx * sy * cz - sx * cy * sz,
      sx * sy * cz + cx * cy * sz,
      sx * sy * sz + cx * cy * cz
    );
  case EulerOrder::YXZ:
    return Quaternion(
      cx * sy * sz + sx * cy * cz,
      -sx * cy * sz + cx * sy * cz,
      cx * cy * sz - sx * sy * cz,
      sx * sy * sz + cx * cy * cz
    );
  case EulerOrder::YZX:
    return Quaternion(
      sx * cy * cz + cx * sy * sz,
      sx * cy * sz + cx * sy * cz,
      -sx * sy * cz + cx * cy * sz,
      -sx * sy * sz + cx * cy * cz
    );
  case EulerOrder::ZXY:
    return Quaternion(
      -cx * sy * sz + sx * cy * cz,
      cx * sy * cz + sx * cy * sz,
      sx * sy * cz + cx * cy * sz,
      -sx * sy * sz + cx * cy * cz
    );
  case EulerOrder::ZYX:
    return Quaternion(
      sx * cy * cz - cx * sy * sz,
      sx * cy * sz + cx * sy * cz,
      -sx * sy * cz + cx * cy * sz,
      sx * sy * sz + cx * cy * cz
    );
  }
  throw "conversion of euler angle to quaterion is failed.";
};

// Convert between quaternions and rotation matrices
RotationMatrix Quaternion::toRotationMatrix(Quaternion q) {
  auto xy2 = q.x * q.y * 2;
  auto xz2 = q.x * q.z * 2;
  auto xw2 = q.x * q.w * 2;
  auto yz2 = q.y * q.z * 2;
  auto yw2 = q.y * q.w * 2;
  auto zw2 = q.z * q.w * 2;
  auto ww2 = q.w * q.w * 2;
  return RotationMatrix({
    ww2 + 2 * q.x * q.x - 1, xy2 + zw2, xz2 - yw2,
    xy2 - zw2, ww2 + 2 * q.y * q.y - 1, yz2 + xw2,
    xz2 + yw2, yz2 - xw2, ww2 + 2 * q.z * q.z - 1 
  });
};

// Convert between rotation matrices and quaternions
Quaternion Quaternion::toQuaternion(RotationMatrix m) {
  auto px = m.at(0, 0) - m.at(1, 1) - m.at(2, 2) + 1;
  auto py = -m.at(0, 0) + m.at(1, 1) - m.at(2, 2) + 1;
  auto pz = -m.at(0, 0) - m.at(1, 1) + m.at(2, 2) + 1;
  auto pw = m.at(0, 0) + m.at(1, 1) + m.at(2, 2) + 1;

  auto selected = 0;
  auto max = px;
  if (max < py) {
    selected = 1;
    max = py;
  }
  if (max < pz) {
    selected = 2;
    max = pz;
  }
  if (max < pw) {
    selected = 3;
    max = pw;
  }

  if (selected == 0) {
    auto x = std::sqrt(px) * 0.5f;
    auto d = 1 / (4 * x);
    return Quaternion(
      x,
      (m.at(1, 0) + m.at(0, 1)) * d,
      (m.at(0, 2) + m.at(2, 0)) * d,
      (m.at(2, 1) - m.at(1, 2)) * d
    );
  } else if (selected == 1) {
    auto y = std::sqrt(py) * 0.5f;
    auto d = 1 / (4 * y);
    return Quaternion(
      (m.at(1, 0) + m.at(0, 1)) * d,
      y,
      (m.at(2, 1) + m.at(1, 2)) * d,
      (m.at(0, 2) - m.at(2, 0)) * d
    );
  } else if (selected == 2) {
    auto z = std::sqrt(pz) * 0.5f;
    auto d = 1 / (4 * z);
    return Quaternion(
      (m.at(0, 2) + m.at(2, 0)) * d,
      (m.at(2, 1) + m.at(1, 2)) * d,
      z,
      (m.at(1, 0) - m.at(0, 1)) * d
    );
  } else if (selected == 3) {
    auto w = std::sqrt(pw) * 0.5f;
    auto d = 1 / (4 * w);
    return Quaternion(
      (m.at(2, 1) - m.at(1, 2)) * d,
      (m.at(0, 2) - m.at(2, 0)) * d,
      (m.at(1, 0) - m.at(0, 1)) * d,
      w
    );
  }
  throw "conversion of rotation matrix to quaterion is failed.";
};

// Convert between quaternions and Euler angles
EulerAngle Quaternion::toEulerAngle(Quaternion q, EulerOrder order) {
  if (order == EulerOrder::XYZ) {
    auto sy = 2 * q.x * q.z + 2 * q.y * q.w;
    auto unlocked = std::abs(sy) < 0.99999f;
    return EulerAngle(
      unlocked ? std::atan2(-(2 * q.y * q.z - 2 * q.x * q.w), 2 * q.w * q.w + 2 * q.z * q.z - 1)
        : std::atan2(2 * q.y * q.z + 2 * q.x * q.w, 2 * q.w * q.w + 2 * q.y * q.y - 1),
      std::asin(sy),
      unlocked ? std::atan2(-(2 * q.x * q.y - 2 * q.z * q.w), 2 * q.w * q.w + 2 * q.x * q.x - 1) : 0,
      order
    );
  } else if (order == EulerOrder::XZY) {
    auto sz = -(2 * q.x * q.y - 2 * q.z * q.w);
    auto unlocked = std::abs(sz) < 0.99999f;
    return EulerAngle(
      unlocked ? std::atan2(2 * q.y * q.z + 2 * q.x * q.w, 2 * q.w * q.w + 2 * q.y * q.y - 1)
        : std::atan2(-(2 * q.y * q.z - 2 * q.x * q.w), 2 * q.w * q.w + 2 * q.z * q.z - 1),
      unlocked ? std::atan2(2 * q.x * q.z + 2 * q.y * q.w, 2 * q.w * q.w + 2 * q.x * q.x - 1) : 0,
      std::asin(sz),
      order
    );
  } else if (order == EulerOrder::YXZ) {
    auto sx = -(2 * q.y * q.z - 2 * q.x * q.w);
    auto unlocked = std::abs(sx) < 0.99999f;
    return EulerAngle(
      std::asin(sx),
      unlocked ? std::atan2(2 * q.x * q.z + 2 * q.y * q.w, 2 * q.w * q.w + 2 * q.z * q.z - 1)
        : std::atan2(-(2 * q.x * q.z - 2 * q.y * q.w), 2 * q.w * q.w + 2 * q.x * q.x - 1),
      unlocked ? std::atan2(2 * q.x * q.y + 2 * q.z * q.w, 2 * q.w * q.w + 2 * q.y * q.y - 1) : 0,
      order
    );
  } else if (order == EulerOrder::YZX) {
    auto sz = 2 * q.x * q.y + 2 * q.z * q.w;
    auto unlocked = std::abs(sz) < 0.99999f;
    return EulerAngle(
      unlocked ? atan2(-(2 * q.y * q.z - 2 * q.x * q.w), 2 * q.w * q.w + 2 * q.y * q.y - 1) : 0,
      unlocked ? atan2(-(2 * q.x * q.z - 2 * q.y * q.w), 2 * q.w * q.w + 2 * q.x * q.x - 1)
        : atan2(2 * q.x * q.z + 2 * q.y * q.w, 2 * q.w * q.w + 2 * q.z * q.z - 1),
      std::asin(sz),
      order
    );
  } else if (order == EulerOrder::ZXY) {
    auto sx = 2 * q.y * q.z + 2 * q.x * q.w;
    auto unlocked = std::abs(sx) < 0.99999f;
    return EulerAngle(
      std::asin(sx),
      unlocked ? std::atan2(-(2 * q.x * q.z - 2 * q.y * q.w), 2 * q.w * q.w + 2 * q.z * q.z - 1) : 0,
      unlocked ? std::atan2(-(2 * q.x * q.y - 2 * q.z * q.w), 2 * q.w * q.w + 2 * q.y * q.y - 1)
        : std::atan2(2 * q.x * q.y + 2 * q.z * q.w, 2 * q.w * q.w + 2 * q.x * q.x - 1),
      order
    );
  } else if (order == EulerOrder::ZYX) {
    auto sy = -(2 * q.x * q.z - 2 * q.y * q.w);
    auto unlocked = std::abs(sy) < 0.99999f;
    return EulerAngle(
      unlocked ? std::atan2(2 * q.y * q.z + 2 * q.x * q.w, 2 * q.w * q.w + 2 * q.z * q.z - 1) : 0,
      std::asin(sy),
      unlocked ? std::atan2(2 * q.x * q.y + 2 * q.z * q.w, 2 * q.w * q.w + 2 * q.x * q.x - 1)
        : std::atan2(-(2 * q.x * q.y - 2 * q.z * q.w), 2 * q.w * q.w + 2 * q.y * q.y - 1),
      order
    );
  }
  throw "conversion of quaternion to euler angle is failed.";
};
