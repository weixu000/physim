#include "SPHSimulator.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

using namespace glm;

SPHSimulator::SPHSimulator(const glm::uvec3& size) {
  const auto transform = translate(vec3(0.f, 2.f, 0.f)) *
                         translate(-vec3(size - 1U) * h / 2.f) * scale(vec3(h));
  for (unsigned i = 0; i < size.x; ++i) {
    for (unsigned j = 0; j < size.y; ++j) {
      for (unsigned k = 0; k < size.z; ++k) {
        const auto index = uvec3(i, j, k);
        Particle p;
        p.p = transform * vec4(vec3(index), 1.f);
        p.v = p.f = vec3(0.f);
        p.rho = rho_0;
        p.m = pow(h, 3) * rho_0;  // Initial mass
        system_.Add(p);
      }
    }
  }

  // Iteratively solve mass to correct initial density
  search_.Update(system_, 2 * h);
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
      for (const auto j : search_.neighbor[i]) {
        if (j == i) continue;
        rho += system_[j].m * W(i, j);
      }
      system_[i].m = (rho_0 - rho) / W(i, i);
    }
  }

  pressure_.resize(system_.Size());
}

void SPHSimulator::Update(float dt) {
  search_.Update(system_, 2 * h);

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
