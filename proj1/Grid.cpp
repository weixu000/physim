#include "Grid.hpp"

#include <glm/gtx/component_wise.hpp>

using namespace glm;

Grid::Grid(const glm::vec3& dimension, const glm::uvec3& size, float mass)
    : dimension_(dimension),
      mass_(mass),
      volume_(dimension.x * dimension.y * dimension.z),
      size_(size),
      stride_(size.y * size.z, size.z, 1) {
  SetupGrid();
  LinkTetrahedra();
}

void Grid::Update(float dt) {
  DeformTetrahedra();

  for (auto& p : particles_) {
    const auto a = p.force / p.mass;
    p.vel += a * dt;
    p.pos += p.vel * dt;
    p.force = vec3(0.f);
  }
}

void Grid::SetupGrid() {
  const auto delta = dimension_ / vec3(size_ - 1U);
  for (unsigned i = 0; i < size_.x; ++i) {
    for (unsigned j = 0; j < size_.y; ++j) {
      for (unsigned k = 0; k < size_.z; ++k) {
        const auto index = uvec3(i, j, k);
        Particle p;
        p.pos = vec3(index) * delta;
        p.vel = p.force = vec3(0.f);
        particles_.push_back(p);
      }
    }
  }
}

void Grid::LinkTetrahedra() {
  for (unsigned i = 0; i < size_.x - 1; ++i) {
    for (unsigned j = 0; j < size_.y - 1; ++j) {
      for (unsigned k = 0; k < size_.y - 1; ++k) {
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
        AddTetrahedron(v[1][1][1], v[1][1][0], v[0][0][1], v[1][0][0]);
        AddTetrahedron(v[1][0][0], v[1][1][1], v[0][0][1], v[0][1][0]);
      }
    }
  }
}

void Grid::DeformTetrahedra() {
  using namespace glm;
  const auto I = mat3();
  for (auto& tt : tetrahedra_) {
    const auto T = GetTetrahedralFrame(tt);
    const auto F = T * tt.R_inv;
    const auto epsilon = (transpose(F) * F - I);
    const auto sigma =
        2 * mu_ * epsilon +
        lambda_ * (epsilon[0][0] + epsilon[1][1] + epsilon[2][2]) * I;
    for (int i = 0; i < 3; ++i) {
      particles_[tt.verts[i]].force += -F * sigma * tt.rest_n[i];
    }
  }
}

void Grid::AddTetrahedron(const glm::uvec3& v0, const glm::uvec3& v1,
                          const glm::uvec3& v2, const glm::uvec3& v3) {
  Tetrahedron tt;
  tt.verts[0] = compAdd(v0 * stride_);
  tt.verts[1] = compAdd(v1 * stride_);
  tt.verts[2] = compAdd(v2 * stride_);
  tt.verts[3] = compAdd(v3 * stride_);

  const auto R = GetTetrahedralFrame(tt);
  tt.R_inv = inverse(R);
  tt.rest_n[0] = cross(R[2], R[1]);
  tt.rest_n[1] = cross(R[0], R[2]);
  tt.rest_n[2] = cross(R[1], R[0]);
  tt.rest_n[3] = cross(R[1] - R[0], R[2] - R[0]);
  tetrahedra_.push_back(tt);

  const auto m_p = mass_ * dot(cross(R[0], R[1]), R[2]) / 6 / volume_;
  for (auto v : tt.verts) {
    particles_[v].mass += m_p;
  }
}

glm::mat3 Grid::GetTetrahedralFrame(const Grid::Tetrahedron& tt) const {
  return mat3(particles_[tt.verts[0]].pos - particles_[tt.verts[3]].pos,
              particles_[tt.verts[1]].pos - particles_[tt.verts[3]].pos,
              particles_[tt.verts[2]].pos - particles_[tt.verts[3]].pos);
}
