#include "RigidBody.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_cross_product.hpp>

using namespace glm;

RigidBody::RigidBody(const glm::vec3& center, const glm::mat3& attitude,
                     const glm::vec3& size, float mass)
    : r_{center}, A_(attitude), size_{size}, m_{mass} {
  I_ = mat3{0.f};
  I_[0][0] = m_ / 12 * (pow(size[1], 2.f) + pow(size[2], 2.f));
  I_[1][1] = m_ / 12 * (pow(size[0], 2.f) + pow(size[2], 2.f));
  I_[2][2] = m_ / 12 * (pow(size[0], 2.f) + pow(size[1], 2.f));
}

glm::mat4 RigidBody::GetTransform() const { return translate(r_) * mat4(A_); }

void RigidBody::AddForce(const vec3& f, const vec3& r) {
  f_ += f;
  M_ += cross(r - r_, f);
}

void RigidBody::Update(float dt) {
  p_ += f_ * dt;
  r_ += p_ / m_ * dt;

  L_ += M_ * dt;
  const auto I = A_ * I_ * transpose(A_);
  const auto omega = inverse(I) * L_;
  A_ += matrixCross3(omega) * A_ * dt;

  f_ = vec3{0.f};
  M_ = vec3{0.f};
}
