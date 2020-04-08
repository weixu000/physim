#pragma once
#include <glm/glm.hpp>

struct Particle {
  void Update(float dt) {
    using namespace glm;
    if (pos.y <= 0) {
      // Elastic collision with y=0
      force.y = max(0.f, force.y);
      vel.y = pos.y = 0;
    }
    const auto a = force / mass;
    vel += a * dt;
    pos += vel * dt;

    vel *= 0.99f;  // Damp velocity
  }

  glm::vec3 pos, vel, force;
  float mass;

  static inline glm::vec3 g{0.f, -9.8f, 0.f};
};