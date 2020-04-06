#include "Axes.hpp"

#include <glbinding/gl/gl.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/StaticStringSource.h>
#include <globjects/globjects.h>

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Camera.hpp"

using namespace gl;
using namespace globjects;

namespace {
StaticStringSource vertex_src(R"(
#version 450

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;

void main() {
    gl_Position = projection * view * world * vec4(position, 1.0);
}
)");
StaticStringSource fragment_src(R"(
#version 450

uniform vec3 color;

out vec4 fragColor;

void main() {
    fragColor = vec4(color, 1.0f);
}
)");

std::unique_ptr<Buffer> vbo;
std::unique_ptr<VertexArray> vao;

std::unique_ptr<Shader> vertex_shader, fragment_shader;
std::unique_ptr<Program> program;
}  // namespace

Axes::Axes() {
  if (!vbo) {
    std::array<glm::vec3, 4> vertices = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
    };
    vbo = Buffer::create();
    vbo->setStorage(vertices, GL_CLIENT_STORAGE_BIT);

    vao = VertexArray::create();
    vao->binding(0)->setAttribute(0);
    vao->binding(0)->setBuffer(vbo.get(), 0, sizeof(glm::vec3));
    vao->binding(0)->setFormat(2, GL_FLOAT);
    vao->enable(0);

    vertex_shader = Shader::create(GL_VERTEX_SHADER, &vertex_src);
    fragment_shader = Shader::create(GL_FRAGMENT_SHADER, &fragment_src);

    program = Program::create();
    program->attach(vertex_shader.get(), fragment_shader.get());
    program->link();
  }
}

void Axes::Draw(const Camera &camera) {
  program->use();
  program->setUniform("projection", camera.Projection());
  program->setUniform("view", camera.View());

  vao->bind();

  program->setUniform("world", transform_);
  program->setUniform("color", glm::vec3(1.0f, 0.0f, 0.0f));
  vao->drawArrays(GL_LINES, 0, 2);

  program->setUniform("world",
                      transform_ * glm::eulerAngleZ(glm::pi<float>() / 2));
  program->setUniform("color", glm::vec3(0.0f, 1.0f, 0.0f));
  vao->drawArrays(GL_LINES, 0, 2);

  program->setUniform("world",
                      transform_ * glm::eulerAngleY(-glm::pi<float>() / 2));
  program->setUniform("color", glm::vec3(0.0f, 0.0f, 1.0f));
  vao->drawArrays(GL_LINES, 0, 2);
}
