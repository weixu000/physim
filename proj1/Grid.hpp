#pragma once
#include <array>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Particle.hpp"

class Grid {
public:
  Grid(const glm::vec3& origin, const glm::vec3& dimension);

  void Update(float dt);

  const std::vector<Particle>& Particles() const { return particles_; }

private:
  struct Tetrahedron {
    std::array<size_t, 4> verts;
    std::array<glm::vec3, 4> rest_n;
    glm::mat3 R_inv;
  };

  /**
   * Fill particles_
   */
  void SetupGrid();

  /**
   * Loop each cell and create 5 tetrahedra by calling AddTetrahedron()
   */
  void LinkTetrahedra();

  /**
   * Add tetrahedron formed by v0, v1, v2, v3
   * Also distribute mass
   */
  void AddTetrahedron(const glm::uvec3& v0, const glm::uvec3& v1,
                      const glm::uvec3& v2, const glm::uvec3& v3);

  glm::mat3 GetTetrahedralFrame(const Tetrahedron& tt) const;

  /**
   * Compute strain-stress relationship
   */
  void DeformTetrahedra();

  // Geometry parameters
  glm::vec3 origin_, dimension_;

  // Grid parameters
  glm::uvec3 size_, stride_;
  std::vector<Particle> particles_;
  std::vector<Tetrahedron> tetrahedra_;

  // Material parameters
  float mu_, lambda_;
  float density_;
};
