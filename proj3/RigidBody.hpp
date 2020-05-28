#pragma once
#include <glm/glm.hpp>

class RigidBody {
public:
  RigidBody() = default;

  RigidBody(const glm::vec3& center, const glm::mat3& attitude,
            const glm::vec3& size, float mass);

  glm::mat4 GetTransform() const;

  void AddForce(const glm::vec3& f, const glm::vec3& r);

  void Update(float dt);

private:
  glm::vec3 r_{0.f};
  glm::vec3 p_{0.f};
  glm::vec3 f_{0.f};

  glm::mat3 A_{1.f};
  glm::vec3 L_{0.f};
  glm::vec3 M_{0.f};

  glm::vec3 size_;
  glm::mat3 I_{};
  float m_;
};
