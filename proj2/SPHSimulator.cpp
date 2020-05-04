#include "SPHSimulator.hpp"

#include <iostream>

using namespace glm;

SPHSimulator::SPHSimulator(const glm::vec3& min_bound,
                           const glm::vec3& max_bound,
                           const ShapeIndicator& indicator) {
  auto sample = min_bound;
  for (sample.x = min_bound.x; sample.x < max_bound.x; sample.x += h) {
    for (sample.y = min_bound.y; sample.y < max_bound.y; sample.y += h) {
      for (sample.z = min_bound.z; sample.z < max_bound.z; sample.z += h) {
        if (indicator(sample)) {
          Particle p;
          p.p = sample;
          p.v = p.f = vec3(0.f);
          p.rho = rho_0;
          p.m = pow(h, 3) * rho_0;  // Initial mass
          system_.Add(p);
        }
      }
    }
  }
  std::cout << "Number of particles: " << system_.Size() << std::endl;

  search_ = NeighborSearch(500, system_.Size(), 2 * h);
  pressure_.resize(system_.Size());

  InitializeMass();
}

void SPHSimulator::InitializeMass() {
  // Iteratively solve mass to correct initial density
  search_.Update(system_);
  for (;;) {
    {
      auto error = -FLT_MAX;
      for (size_t i = 0; i < system_.Size(); ++i) {
        error = max(
            error,
            abs(Value(i, [this](const size_t j) { return system_[j].rho; }) -
                rho_0));
      }
      std::cout << "Density error: " << error / rho_0 << std::endl;
      if (error / rho_0 < 1E-3f) break;
    }

    // https://en.wikipedia.org/wiki/Jacobi_method
    for (size_t i = 0; i < system_.Size(); ++i) {
      auto rho = 0.f;
      for (const auto j : search_.neighbors[i]) {
        if (j == i) continue;
        rho += system_[j].m * W(i, j);
      }
      system_[i].m = (rho_0 - rho) / W(i, i);
    }
  }
}

void SPHSimulator::Update(float dt) {
  search_.Update(system_);

  for (size_t i = 0; i < system_.Size(); ++i) {
    system_[i].rho =
        Value(i, [this](const size_t j) { return system_[j].rho; });
    pressure_[i] = k * (pow(system_[i].rho / rho_0, 7) - 1);
  }

  for (size_t i = 0; i < system_.Size(); ++i) {
    const auto f_pressure = -system_[i].m / system_[i].rho *
                            Grad(i, [this](size_t j) { return pressure_[j]; });
    const auto f_viscosity = system_[i].m * nu * Laplace(i, [this](size_t j) {
                               return system_[j].v;
                             });
    const auto f_gravity = system_[i].m * ParticleSystem::g;
    system_[i].f = f_pressure + f_viscosity + f_gravity;
  }

  // Box interaction
  for (size_t i = 0; i < system_.Size(); ++i) {
    system_[i].f +=
        box_stiffness_ * system_[i].m * max(0.f, -system_[i].p.y) *
            vec3{0.f, 1.f, 0.f} +
        box_stiffness_ * system_[i].m * max(0.f, system_[i].p.x - box_x_) *
            vec3{-1.f, 0.f, 0.f} +
        box_stiffness_ * system_[i].m * max(0.f, system_[i].p.z - box_z_) *
            vec3{0.f, 0.f, -1.f} +
        box_stiffness_ * system_[i].m * max(0.f, -system_[i].p.x - box_x_) *
            vec3{1.f, 0.f, 0.f} +
        box_stiffness_ * system_[i].m * max(0.f, -system_[i].p.z - box_z_) *
            vec3{0.f, 0.f, 1.f};
  }

  integrator_.Integrate(system_, dt);
}
