#include "Axes.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glpp/buffer.hpp>
#include <glpp/program.hpp>
#include <glpp/vertexarray.hpp>
#include <memory>

#include "Camera.hpp"

using namespace glm;
using namespace glpp;

namespace {
const auto vertex_src = R"(
#version 450

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;

void main() {
    gl_Position = projection * view * world * vec4(position, 1.0);
}
)";
const auto fragment_src = R"(
#version 450

uniform vec3 color;

out vec4 fragColor;

void main() {
    fragColor = vec4(color, 1.0f);
}
)";

std::unique_ptr<VertexArray> vao;
std::unique_ptr<Program> program;
}  // namespace

Axes::Axes() {
  if (!vao) {
    std::array<vec3, 4> vertices = {
        vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f),
    };
    Buffer vbo;
    vbo.CreateStorage(vertices, GL_CLIENT_STORAGE_BIT);

    vao = std::make_unique<VertexArray>();
    vao->BindVertexBuffer(0, vbo, sizeof(vec3), 0);
    vao->EnableAttrib(0);
    vao->AttribBinding(0, 0);
    vao->AttribFormat<vec3>(0, 0);

    program = std::make_unique<Program>(Shader(VERTEX_SHADER, vertex_src),
                                        Shader(FRAGMENT_SHADER, fragment_src));
  }
}

void Axes::Draw(const Camera &camera) {
  program->Use();
  program->Uniform("projection", camera.Projection());
  program->Uniform("view", camera.View());

  vao->Bind();

  program->Uniform("world", transform_);
  program->Uniform("color", vec3(1.0f, 0.0f, 0.0f));
  glDrawArrays(GL_LINES, 0, 2);

  program->Uniform("world", transform_ * eulerAngleZ(pi<float>() / 2));
  program->Uniform("color", vec3(0.0f, 1.0f, 0.0f));
  glDrawArrays(GL_LINES, 0, 2);

  program->Uniform("world", transform_ * eulerAngleY(-pi<float>() / 2));
  program->Uniform("color", vec3(0.0f, 0.0f, 1.0f));
  glDrawArrays(GL_LINES, 0, 2);
}
