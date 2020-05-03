#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Integrator.hpp"
#include "NeighborSearch.hpp"
#include "ParticleSystem.hpp"

class SPHSimulator {
public:
  SPHSimulator() = default;

  using ShapeIndicator = std::function<bool(const glm::vec3&)>;

  SPHSimulator(const glm::vec3& min_bound, const glm::vec3& max_bound,
               const ShapeIndicator& indicator);

  void Update(float dt);

  const ParticleSystem& GetParticles() const { return system_; }

  glm::vec3 GetBox() const { return {box_x_, 10.f, box_z_}; }

private:
  void InitializeMass();

  float f(float q) const {
    const auto c = 3.f / 2 / glm::pi<float>();
    if (q < 1) {
      return c * (2.f / 3 - pow(q, 2) + pow(q, 3) / 2);
    } else if (q < 2) {
      return c * pow(2 - q, 3) / 6;
    } else {
      return 0;
    }
  }

  float Df(float q) const {
    const auto c = 3.f / 2 / glm::pi<float>();
    if (q < 1) {
      return c * (-2 * q + pow(q, 2) * 3 / 2);
    } else if (q < 2) {
      return c * -pow(2 - q, 2) / 2;
    } else {
      return 0;
    }
  }

  float W(size_t i, size_t j) const {
    const auto q = glm::length(system_[i].p - system_[j].p) / h;
    return f(q) / pow(h, 3);
  }

  glm::vec3 DelW(size_t i, size_t j) const {
    const auto d = system_[i].p - system_[j].p;
    const auto dd = glm::length(d);
    if (dd) {
      return Df(dd / h) * d / dd / pow(h, 4);
    } else {
      return glm::vec3{0.f};
    }
  }

  template <typename T>
  auto Value(size_t i, T a) const {
    decltype(a(0)) ret{0.f};
    for (const auto j : search_.neighbors[i]) {
      ret += system_[j].m / system_[j].rho * a(j) * W(i, j);
    }
    return ret;
  }

  template <typename T>
  glm::vec3 Grad(size_t i, T a) const {
    glm::vec3 ret{0.f};
    for (const auto j : search_.neighbors[i]) {
      ret += system_[j].m *
             (a(i) / pow(system_[i].rho, 2) + a(j) / pow(system_[j].rho, 2)) *
             DelW(i, j);
    }
    return system_[i].rho * ret;
  }

  template <typename T>
  auto Laplace(size_t i, T a) const {
    decltype(a(0)) ret{0.f};
    for (const auto j : search_.neighbors[i]) {
      const auto x_ij = system_[i].p - system_[j].p;
      ret += system_[j].m / system_[j].rho * (a(i) - a(j)) *
             glm::dot(x_ij, DelW(i, j)) /
             (glm::dot(x_ij, x_ij) + .01f * pow(h, 2));
    }
    return 2.f * ret;
  }

  ParticleSystem system_;
  NeighborSearch search_;
  Integrator integrator_;

  std::vector<float> pressure_;

  float h = 0.1f, k = 1119E3f, rho_0 = 1E3f, nu = 1E-2f;

  float box_x_ = 1.f, box_z_ = 1.f, box_stiffness_ = 1E5f;
};
