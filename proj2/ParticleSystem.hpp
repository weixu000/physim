#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Particle {
  glm::vec3 p, v, f;
  float m, rho;
};

class ParticleSystem {
public:
  size_t Size() const { return data.size(); }

  const Particle& operator[](size_t i) const { return data[i]; }
  Particle& operator[](size_t i) { return data[i]; }

  void Add(const Particle& p) { data.push_back(p); }

  std::vector<Particle> data;
  static inline const glm::vec3 g{0.f, -9.8f, 0.f};
};
