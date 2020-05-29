#include "RigidBody.hpp"

#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

RigidBody::RigidBody(const glm::vec3& center, const glm::mat3& attitude,
                     const glm::vec3& L, const glm::vec3& size, float mass)
    : r_{center}, A_(attitude), L_(L), size_{size}, m_{mass} {
  I_ = mat3{0.f};
  I_[0][0] = m_ / 12 * (pow(size[1], 2.f) + pow(size[2], 2.f));
  I_[1][1] = m_ / 12 * (pow(size[0], 2.f) + pow(size[2], 2.f));
  I_[2][2] = m_ / 12 * (pow(size[0], 2.f) + pow(size[1], 2.f));
}

glm::mat4 RigidBody::GetTransform() const { return translate(r_) * mat4(A_); }

void RigidBody::Update(float dt) {
  p_ += f_ * dt;
  r_ += p_ / m_ * dt;

  L_ += M_ * dt;
  const auto omega = GetOmega();
  if (glm::length(omega) != 0.f) {
    A_ = mat3(axisAngleMatrix(omega, length(omega) * dt)) * A_;
  }

  f_ = vec3{0.f};
  M_ = vec3{0.f};
}
