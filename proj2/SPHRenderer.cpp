#include "SPHRenderer.hpp"

#include <glad/glad.h>

#include <array>
#include <glpp/program.hpp>

#include "Camera.hpp"
#include "proj2_shaders.hpp"

using namespace glm;
using namespace glpp;

namespace {
std::unique_ptr<Program> program;
std::unique_ptr<Buffer> box_ebo;

void Initialize() {
  if (!program) {
    program = std::make_unique<Program>(
        Shader(VERTEX_SHADER, std::string(sph_vert.begin(), sph_vert.end())),
        Shader(FRAGMENT_SHADER, std::string(sph_frag.begin(), sph_frag.end())));
  }
  if (!box_ebo) {
    std::array<uint, 24> indices = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                                    6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
    box_ebo = std::make_unique<Buffer>();
    box_ebo->CreateStorage(indices, GL_CLIENT_STORAGE_BIT);
  }
}
}  // namespace

SPHRenderer::SPHRenderer(const ParticleSystem& system, const glm::vec3& box)
    : size_(system.Size()) {
  Initialize();

  vbo_ = std::make_unique<Buffer>();
  vbo_->CreateStorage(system.data, GL_DYNAMIC_STORAGE_BIT);

  vao_ = std::make_unique<VertexArray>();
  vao_->BindVertexBuffer(0, *vbo_, sizeof(Particle), 0);
  vao_->EnableAttrib(0, 1, 2, 3, 4);
  vao_->AttribBinding(0, 0, 1, 2, 3, 4);
  vao_->AttribFormat<vec3>(0, 0);
  vao_->AttribFormat<vec3>(1, sizeof(vec3));
  vao_->AttribFormat<vec3>(2, 2 * sizeof(vec3));
  vao_->AttribFormat<float>(3, 3 * sizeof(vec3));
  vao_->AttribFormat<float>(4, 4 * sizeof(vec3) + sizeof(float));

  std::array<vec3, 8> box_vertices = {
      vec3{-box.x, 0, -box.z},     vec3{box.x, 0, -box.z},
      vec3{box.x, 0, box.z},       vec3{-box.x, 0, box.z},
      vec3{-box.x, box.y, -box.z}, vec3{box.x, box.y, -box.z},
      vec3{box.x, box.y, box.z},   vec3{-box.x, box.y, box.z}};
  Buffer box_vbo;
  box_vbo.CreateStorage(box_vertices, GL_CLIENT_STORAGE_BIT);
  box_vao_ = std::make_unique<VertexArray>();
  box_vao_->BindVertexBuffer(0, box_vbo, sizeof(vec3), 0);
  box_vao_->BindElementBuffer(*box_ebo);
  box_vao_->EnableAttrib(0);
  box_vao_->AttribBinding(0, 0);
  box_vao_->AttribFormat<vec3>(0, 0);
}

void SPHRenderer::Update(const ParticleSystem& system) {
  assert(system.Size() == size_);
  vbo_->SetSubData(system.data);
}

void SPHRenderer::Draw(const Camera& camera) {
  program->Use();
  program->Uniform("projection", camera.Projection());
  program->Uniform("view", camera.View());

  vao_->Bind();
  glDrawArrays(GL_POINTS, 0, size_);

  box_vao_->Bind();
  glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
}
