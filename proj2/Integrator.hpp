#pragma once

#include "ParticleSystem.hpp"

class Integrator {
public:
  void Integrate(ParticleSystem& system, float dt);
};
