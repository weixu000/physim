#pragma once

#include "ParticleSystem.hpp"

class NeighborSearch {
public:
  NeighborSearch() = default;

  NeighborSearch(size_t m, size_t n) : neighbors(n), buckets_(m) {}

  void Update(const ParticleSystem& system, float d);

  std::vector<std::vector<size_t>> neighbors;

private:
  std::vector<std::vector<size_t>> buckets_;
};
