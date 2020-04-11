#pragma once
#include <glm/glm.hpp>

struct Particle {
  void Update(float dt) {
    using namespace glm;
    const auto a = force / mass;
    vel += a * dt;
    pos += vel * dt;
    if (pos.y <= 0) {
      // Collision with y=0
      vel.y = pos.y = 0;
    }
  }

  glm::vec3 pos, vel, force;
  float mass;

  static inline glm::vec3 g{0.f, -9.8f, 0.f};
};