#include "GridRenderer.hpp"

#include <glbinding/gl/gl.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/StaticStringSource.h>
#include <globjects/globjects.h>

#include "Camera.hpp"
#include "Particle.hpp"

using namespace glm;
using namespace gl;
using namespace globjects;

namespace {
StaticStringSource vertex_src(R"(
#version 450

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vel;
layout (location = 2) in vec3 force;
layout (location = 3) in float mass;

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(pos, 1.0);
}
)");
StaticStringSource fragment_src(R"(
#version 450

out vec4 fragColor;

void main() {
    fragColor = vec4(1.0);
}
)");
std::unique_ptr<Shader> vertex_shader, fragment_shader;
std::unique_ptr<Program> program;
}  // namespace

GridRenderer::GridRenderer(const std::vector<Particle>& particles)
    : size_(particles.size()) {
  vbo = Buffer::create();
  vbo->setStorage(particles, GL_DYNAMIC_STORAGE_BIT);

  vao = VertexArray::create();
  vao->binding(0)->setAttribute(0);
  vao->binding(0)->setBuffer(vbo.get(), 0, sizeof(Particle));
  vao->binding(0)->setFormat(3, GL_FLOAT);
  vao->enable(0);
  vao->binding(0)->setAttribute(1);
  vao->binding(0)->setFormat(3, GL_FLOAT, GL_FALSE, sizeof(vec3));
  vao->enable(1);
  vao->binding(0)->setAttribute(2);
  vao->binding(0)->setFormat(3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3));
  vao->enable(2);
  vao->binding(0)->setAttribute(3);
  vao->binding(0)->setFormat(1, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3));
  vao->enable(3);

  if (!program) {
    vertex_shader = Shader::create(GL_VERTEX_SHADER, &vertex_src);
    fragment_shader = Shader::create(GL_FRAGMENT_SHADER, &fragment_src);
    program = Program::create();
    program->attach(vertex_shader.get(), fragment_shader.get());
    program->link();
  }
}

void GridRenderer::Update(const std::vector<Particle>& particles) {
  assert(particles.size() == size_);
  vbo->setSubData(particles);
}

void GridRenderer::Draw(const Camera& camera) {
  program->use();
  program->setUniform("projection", camera.Projection());
  program->setUniform("view", camera.View());

  vao->bind();
  vao->drawArrays(GL_POINTS, 0, size_);
}
