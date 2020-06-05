#pragma once
#include <glm/glm.hpp>
#include <optional>

#include "RigidBody.hpp"

class Collision {
public:
  void Compute(RigidBody& rb) const;

  float eps_{.5f}, mu_{.5f};

private:
  std::optional<glm::vec3> Detect(const RigidBody& rb) const;
};
