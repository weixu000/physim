#include "Grid.hpp"

#include <glm/gtx/component_wise.hpp>

using namespace glm;

Grid::Grid(const glm::vec3& origin, const glm::vec3& cell,
           const glm::uvec3& size, float E, float nu, float density)
    : origin_(origin), cell_(cell), size_(size), density_(density) {
  stride_ = vec3(size_.y * size_.z, size_.z, 1);

  lambda_ = E * nu / (1 + nu) / (1 - 2 * nu);
  mu_ = E / 2 / (1 + nu);

  SetupGrid();
  LinkTetrahedra();
}

void Grid::SetupGrid() {
  for (unsigned i = 0; i < size_.x; ++i) {
    for (unsigned j = 0; j < size_.y; ++j) {
      for (unsigned k = 0; k < size_.z; ++k) {
        const auto index = uvec3(i, j, k);
        Particle p;
        p.pos = vec3(index) * cell_ + origin_;
        p.vel = vec3(0.f);
        // Computed later
        p.force = vec3(0.f);
        p.mass = 0.f;
        particles_.push_back(p);
      }
    }
  }
}

void Grid::LinkTetrahedra() {
  for (unsigned i = 0; i < size_.x - 1; ++i) {
    for (unsigned j = 0; j < size_.y - 1; ++j) {
      for (unsigned k = 0; k < size_.z - 1; ++k) {
        std::array<std::array<std::array<uvec3, 2>, 2>, 2> v;
        for (unsigned ii = 0; ii < 2; ++ii) {
          for (unsigned jj = 0; jj < 2; ++jj) {
            for (unsigned kk = 0; kk < 2; ++kk) {
              v[ii][jj][kk] = vec3(i + ii, j + jj, k + kk);
            }
          }
        }

        AddTetrahedron(v[0][0][0], v[1][0][0], v[0][0][1], v[0][1][0]);
        AddTetrahedron(v[1][0][0], v[1][0][1], v[0][0][1], v[1][1][1]);
        AddTetrahedron(v[1][1][1], v[0][1][0], v[0][1][1], v[0][0][1]);
        AddTetrahedron(v[1][1][1], v[1][1][0], v[0][1][0], v[1][0][0]);
        AddTetrahedron(v[1][0][0], v[1][1][1], v[0][0][1], v[0][1][0]);
      }
    }
  }

  // Add gravity
  for (auto& p : particles_) {
    p.force = p.mass * Particle::g;
  }
#ifndef NDEBUG
  // Mass is correct
  auto mass = 0.f;
  for (const auto& p : particles_) {
    mass += p.mass;
  }
  assert(mass / (density_ * compMul(dimension_)) - 1 < 1E-3f);
#endif
}

void Grid::AddTetrahedron(const glm::uvec3& v0, const glm::uvec3& v1,
                          const glm::uvec3& v2, const glm::uvec3& v3) {
  Tetrahedron tt;
  const Indices verts = {compAdd(v0 * stride_), compAdd(v1 * stride_),
                         compAdd(v2 * stride_), compAdd(v3 * stride_)};
  const auto R = GetTetrahedralFrame(verts);
  tt.R_inv = inverse(R);
  tt.rest_n[0] = cross(R[2], R[1]) / 2.f;
  tt.rest_n[1] = cross(R[0], R[2]) / 2.f;
  tt.rest_n[2] = cross(R[1], R[0]) / 2.f;
  tt.rest_n[3] = cross(R[1] - R[0], R[2] - R[0]) / 2.f;
#ifndef NDEBUG
  // closed surface normal sums to 0
  auto normal = vec3(0.f);
  for (const auto& n : tt.rest_n) {
    normal += n;
  }
  assert(abs(compAdd(normal)) < 1E-3f);
#endif
  tetrahedra_.push_back(tt);
  vertices_.push_back(verts);

  // Distribute mass to every vertices
  const auto m_p = density_ * dot(cross(R[0], R[1]), R[2]) / 6 / 4;
  assert(m_p >= 0);  // Volume should be positive
  for (auto v : verts) {
    particles_[v].mass += m_p;
  }
}

glm::mat3 Grid::GetTetrahedralFrame(const Indices& verts) const {
  return mat3(particles_[verts[0]].pos - particles_[verts[3]].pos,
              particles_[verts[1]].pos - particles_[verts[3]].pos,
              particles_[verts[2]].pos - particles_[verts[3]].pos);
}

void Grid::Update(float dt) {
  // Add gravity
  for (auto& p : particles_) {
    p.force = p.mass * Particle::g;
  }

  DeformTetrahedra();

  for (auto& p : particles_) {
    p.Update(dt);
  }
}

void Grid::DeformTetrahedra() {
  using namespace glm;
  const auto I = mat3(1.f);
  for (size_t t = 0; t < tetrahedra_.size(); ++t) {
    const auto& tt = tetrahedra_[t];
    const auto T = GetTetrahedralFrame(vertices_[t]);
    const auto F = T * tt.R_inv;
    const auto epsilon = (transpose(F) * F - I) / 2.f;
    const auto sigma =
        2 * mu_ * epsilon +
        lambda_ * (epsilon[0][0] + epsilon[1][1] + epsilon[2][2]) * I;
    for (int i = 0; i < 4; ++i) {
      particles_[vertices_[t][i]].force += sigma * tt.rest_n[i];
    }
  }
}
