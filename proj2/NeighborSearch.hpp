#pragma once

#include "ParticleSystem.hpp"

class NeighborSearch {
public:
  NeighborSearch() = default;

  NeighborSearch(size_t m, size_t n, float d)
      : neighbors(n), d_(d), buckets_(m) {}

  void Update(const ParticleSystem& system);

  const std::vector<size_t>& Search(const ParticleSystem& system,
                                    const glm::vec3& p) const {
    static std::vector<size_t> out;
    Search(system, p, out);
    return out;
  }

  std::vector<std::vector<size_t>> neighbors;

private:
  void Search(const ParticleSystem& system, const glm::vec3& p,
              std::vector<size_t>& out) const;

  float d_;
  std::vector<std::vector<size_t>> buckets_;
};
