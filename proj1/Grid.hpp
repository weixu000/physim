#pragma once
#include <array>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Particle.hpp"

class Grid {
public:
  Grid(const glm::vec3& translation, const glm::vec3& yaw_pitch_roll,
       const glm::vec3& cell, const glm::uvec3& size, float E, float nu,
       float eta, float density = 1.f);

  void Update(float dt);

  const std::vector<Particle>& Particles() const { return particles_; }

  using Indices = std::array<glm::uint, 4>;  // Tetrahedron 4 indices

  const std::vector<Indices>& ParticleIndices() const { return vertices_; }

  void SetElasticParams(float E, float nu) {
    lambda_ = E * nu / (1 + nu) / (1 - 2 * nu);
    mu_ = E / 2 / (1 + nu);
  }

  void SetDamping(float eta) { eta_ = eta; }

private:
  struct Tetrahedron {
    std::array<glm::vec3, 4> rest_n;
    glm::mat3 R_inv;
  };

  /**
   * Fill particles_
   */
  void SetupGrid(const glm::vec3& translation, const glm::vec3& yaw_pitch_roll,
                 const glm::vec3& cell);

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

  glm::mat3 GetTetrahedralFrame(const Indices& verts) const;

  glm::mat3 GetTetrahedralVelocity(const Indices& verts) const;

  /**
   * Compute strain-stress relationship
   */
  void DeformTetrahedra();

  // Grid parameters
  glm::uvec3 size_, stride_;
  std::vector<Particle> particles_;
  std::vector<Tetrahedron> tetrahedra_;
  std::vector<Indices> vertices_;

  // Material parameters
  float mu_, lambda_, eta_;
  float density_;
};
