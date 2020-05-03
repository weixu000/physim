#include "NeighborSearch.hpp"

#include <algorithm>
#include <array>

using namespace glm;
using namespace std;

namespace {
size_t Hash(const glm::vec3& x, float d, size_t m,
            const glm::ivec3& disp = ivec3{0, 0, 0}) {
  const auto c =
      (ivec3(floor(x / d)) + disp) * ivec3{73856093, 19349663, 83492791};
  return size_t(c.x ^ c.y ^ c.z) % m;
}
}  // namespace

void NeighborSearch::Update(const ParticleSystem& system, float d) {
  for (auto& b : buckets_) {
    b.clear();
  }
  for (size_t i = 0; i < system.Size(); ++i) {
    buckets_[Hash(system[i].p, d, buckets_.size())].push_back(i);
  }

  for (size_t i = 0; i < system.Size(); ++i) {
    neighbors[i].clear();

    array<size_t, 27> seen;
    size_t n_seen = 0;
    // Loop over 3*3*3 neighboring cells
    for (int x = -1; x < 2; ++x) {
      for (int y = -1; y < 2; ++y) {
        for (int z = -1; z < 2; ++z) {
          const auto hash =
              Hash(system[i].p, d, buckets_.size(), ivec3{x, y, z});

          // Hash of neighors may be the same, deduplicate
          if (find(begin(seen), begin(seen) + n_seen, hash) !=
              begin(seen) + n_seen) {
            continue;
          }
          seen[n_seen++] = hash;

          for (const auto j : buckets_[hash]) {
            if (length(system[i].p - system[j].p) < d) {
              neighbors[i].push_back(j);
            }
          }
        }
      }
    }
  }
}
