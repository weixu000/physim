#pragma once
#include <glm/glm.hpp>

struct Particle {
  void Update(float dt);

  glm::vec3 pos, vel, force;
  float mass;

  static inline const glm::vec3 g{0.f, -9.8f, 0.f};
  static inline float mu = .4f;
};