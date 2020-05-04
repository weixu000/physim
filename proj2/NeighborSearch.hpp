#pragma once

#include "ParticleSystem.hpp"

class NeighborSearch {
public:
  NeighborSearch() = default;

  NeighborSearch(size_t m, size_t n, float d)
      : neighbors(n), d_(d), buckets_(m) {}

  void Update(const ParticleSystem& system);

  std::vector<std::vector<size_t>> neighbors;

private:
  float d_;
  std::vector<std::vector<size_t>> buckets_;
};
