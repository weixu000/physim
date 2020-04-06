#pragma once
#include <glm/glm.hpp>

struct Particle {
  glm::vec3 pos, vel, force;
  float mass;
};