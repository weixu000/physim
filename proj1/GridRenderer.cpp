#include "GridRenderer.hpp"

#include <glad/glad.h>

#include <glpp/buffer.hpp>
#include <glpp/program.hpp>
#include <glpp/vertexarray.hpp>
#include <shaders.hpp>

#include "Camera.hpp"
#include "Particle.hpp"

using namespace glm;
using namespace glpp;

namespace {
std::unique_ptr<Program> vec_program, tetra_program, surf_program;
}  // namespace

GridRenderer::GridRenderer(const std::vector<Particle>& particles,
                           const std::vector<std::array<uint, 4>>& tetra)
    : size_(particles.size()), tetra_size_(tetra.size()) {
  vbo = std::make_unique<Buffer>();
  ebo = std::make_unique<Buffer>();
  vbo->CreateStorage(particles, GL_DYNAMIC_STORAGE_BIT);
  ebo->CreateStorage(tetra, GL_CLIENT_STORAGE_BIT);

  vao = std::make_unique<VertexArray>();
  vao->BindVertexBuffer(0, *vbo, sizeof(Particle), 0);
  vao->BindElementBuffer(*ebo);
  vao->EnableAttrib(0, 1, 2, 3);
  vao->AttribBinding(0, 0, 1, 2, 3);
  vao->AttribFormat<vec3>(0, 0);
  vao->AttribFormat<vec3>(1, sizeof(vec3));
  vao->AttribFormat<vec3>(2, 2 * sizeof(vec3));
  vao->AttribFormat<float>(3, 3 * sizeof(vec3));

  if (!vec_program) {
    vec_program = std::make_unique<Program>(
        Shader(VERTEX_SHADER, std::string(grid_vert.begin(), grid_vert.end())),
        Shader(FRAGMENT_SHADER,
               std::string(grid_plain_frag.begin(), grid_plain_frag.end())),
        Shader(GEOMETRY_SHADER,
               std::string(grid_vec_geom.begin(), grid_vec_geom.end())));

    tetra_program = std::make_unique<Program>(
        Shader(VERTEX_SHADER, std::string(grid_vert.begin(), grid_vert.end())),
        Shader(FRAGMENT_SHADER,
               std::string(grid_plain_frag.begin(), grid_plain_frag.end())),
        Shader(GEOMETRY_SHADER,
               std::string(grid_tetra_geom.begin(), grid_tetra_geom.end())));
    surf_program = std::make_unique<Program>(
        Shader(VERTEX_SHADER, std::string(grid_vert.begin(), grid_vert.end())),
        Shader(FRAGMENT_SHADER,
               std::string(grid_phong_frag.begin(), grid_phong_frag.end())),
        Shader(GEOMETRY_SHADER,
               std::string(grid_tetra_geom.begin(), grid_tetra_geom.end())));
  }
}

void GridRenderer::Update(const std::vector<Particle>& particles) {
  assert(particles.size() == size_);
  vbo->SetSubData(particles);
}

void GridRenderer::DrawTetrahedra(const Camera& camera) {
  vao->Bind();

  vec_program->Use();
  vec_program->Uniform("projection", camera.Projection());
  vec_program->Uniform("view", camera.View());
  glDrawArrays(GL_POINTS, 0, size_);

  tetra_program->Use();
  tetra_program->Uniform("projection", camera.Projection());
  tetra_program->Uniform("view", camera.View());
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawElements(GL_LINES_ADJACENCY, tetra_size_ * 5, GL_UNSIGNED_INT, nullptr);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GridRenderer::DrawSurface(const Camera& camera) {
  vao->Bind();
  surf_program->Use();
  surf_program->Uniform("projection", camera.Projection());
  surf_program->Uniform("view", camera.View());
  glDrawElements(GL_LINES_ADJACENCY, tetra_size_ * 5, GL_UNSIGNED_INT, nullptr);
}
