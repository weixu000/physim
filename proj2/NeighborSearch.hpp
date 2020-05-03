#pragma once

#include "ParticleSystem.hpp"

class NeighborSearch {
public:
  void Update(const ParticleSystem& system, float d);

  std::vector<std::vector<size_t>> neighbor;
};
