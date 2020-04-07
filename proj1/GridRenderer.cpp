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
uniform vec3 eye;

out vec3 vPos;
out vec3 vVec;
out vec3 vColor;

void main() {
    gl_Position = projection * view * vec4(pos, 1.0);
//    gl_PointSize = mass * 100000 / length(eye - pos);

    vPos = pos;
    vVec = force / mass / 9.8 / 10;
    vColor = vec3(1.0);
}
)");
StaticStringSource fragment_src(R"(
#version 450

in vec3 vColor;

out vec4 fragColor;

void main() {
    fragColor = vec4(vColor, 1.0);
}
)");
StaticStringSource geometry_src(R"(
#version 450
layout (points) in;
layout (line_strip, max_vertices = 2) out;

uniform mat4 projection;
uniform mat4 view;

in vec3 vPos[1];
in vec3 vVec[1];

out vec3 vColor;

void main() {
    gl_Position = projection * view * vec4(vPos[0], 1.0);
    vColor = vec3(1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(vPos[0] + vVec[0], 1.0);
    vColor = vec3(0.0);
    EmitVertex();
    EndPrimitive();
}
)");
std::unique_ptr<Shader> vertex_shader, fragment_shader, geometry_shader;
std::unique_ptr<Program> program, vec_program;
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

    geometry_shader = Shader::create(GL_GEOMETRY_SHADER, &geometry_src);
    vec_program = Program::create();
    vec_program->attach(vertex_shader.get(), fragment_shader.get(),
                        geometry_shader.get());
    vec_program->link();
  }

  glEnable(GL_PROGRAM_POINT_SIZE);  // Point size indicates mass
}

void GridRenderer::Update(const std::vector<Particle>& particles) {
  assert(particles.size() == size_);
  vbo->setSubData(particles);
}

void GridRenderer::Draw(const Camera& camera) {
  vao->bind();

  program->use();
  program->setUniform("projection", camera.Projection());
  program->setUniform("view", camera.View());
  program->setUniform("eye", camera.Eye());
  vao->drawArrays(GL_POINTS, 0, size_);

  vec_program->use();
  vec_program->setUniform("projection", camera.Projection());
  vec_program->setUniform("view", camera.View());
  vec_program->setUniform("eye", camera.Eye());
  vao->drawArrays(GL_POINTS, 0, size_);
}
