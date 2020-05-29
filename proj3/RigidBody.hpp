#pragma once
#include <glm/glm.hpp>

class RigidBody {
public:
  RigidBody() = default;

  RigidBody(const glm::vec3& center, const glm::mat3& attitude,
            const glm::vec3& L, const glm::vec3& size, float mass);

  glm::mat4 GetTransform() const;
  glm::vec3 GetVelocity(const glm::vec3& r_offset) const {
    return p_ / m_ + glm::cross(GetOmega(), r_offset);
  }
  glm::mat3 GetInertia() const { return A_ * I_ * transpose(A_); }

  glm::vec3 GetOffset(const glm::vec3& r) const { return r - r_; }

  void AddForce(const glm::vec3& f, const glm::vec3& r_offset) {
    f_ += f;
    M_ += cross(r_offset, f);
  }
  void AddImpulse(const glm::vec3& j, const glm::vec3& r_offset) {
    p_ += j;
    L_ += cross(r_offset, j);
  }

  void Update(float dt);

  glm::vec3 size_;
  float m_;

private:
  glm::vec3 GetOmega() const { return inverse(GetInertia()) * L_; }

  glm::vec3 r_{0.f};
  glm::vec3 p_{0.f};
  glm::vec3 f_{0.f};

  glm::mat3 A_{1.f};
  glm::vec3 L_{.0f};
  glm::vec3 M_{0.f};

  glm::mat3 I_{};
};
