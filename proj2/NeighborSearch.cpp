#include "NeighborSearch.hpp"

void NeighborSearch::Update(const ParticleSystem& system, float d) {
  neighbor.resize(system.Size());
  for (size_t i = 0; i < system.Size(); ++i) {
    neighbor[i].clear();
    for (size_t j = 0; j < system.Size(); ++j) {
      if (glm::length(system[i].p - system[j].p) < d) {
        neighbor[i].push_back(j);
      }
    }
  }
}
