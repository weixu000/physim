#include "SPHRenderer.hpp"

#include <glad/glad.h>

#include <glpp/program.hpp>

#include "Camera.hpp"
#include "proj2_shaders.hpp"

using namespace glm;
using namespace glpp;

namespace {
std::unique_ptr<Program> program;
}  // namespace

SPHRenderer::SPHRenderer(const ParticleSystem& system) : size_(system.Size()) {
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

  if (!program) {
    program = std::make_unique<Program>(
        Shader(VERTEX_SHADER, std::string(sph_vert.begin(), sph_vert.end())),
        Shader(FRAGMENT_SHADER, std::string(sph_frag.begin(), sph_frag.end())));
  }
}

void SPHRenderer::Update(const ParticleSystem& system) {
  assert(system.Size() == size_);
  vbo_->SetSubData(system.data);
}

void SPHRenderer::Draw(const Camera& camera) {
  vao_->Bind();
  program->Use();
  program->Uniform("projection", camera.Projection());
  program->Uniform("view", camera.View());
  glDrawArrays(GL_POINTS, 0, size_);
}
