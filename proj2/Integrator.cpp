#include "Integrator.hpp"

using namespace glm;

void Integrator::Integrate(ParticleSystem& system, float dt) {
  for (size_t i = 0; i < system.Size(); ++i) {
    const auto a = system[i].f / system[i].m;
    system[i].v += a * dt;
    system[i].p += system[i].v * dt;
  }
}
