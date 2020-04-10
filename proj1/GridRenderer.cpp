#include "GridRenderer.hpp"

#include <glad/glad.h>

#include <glpp/buffer.hpp>
#include <glpp/program.hpp>
#include <glpp/vertexarray.hpp>

#include "Camera.hpp"
#include "Particle.hpp"

using namespace glm;
using namespace glpp;

namespace {
const auto vertex_src = R"(
#version 450

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vel;
layout (location = 2) in vec3 force;
layout (location = 3) in float mass;

uniform mat4 projection;
uniform mat4 view;

out vec3 vPos;
out vec3 vVec;
out vec3 vColor;

void main() {
    gl_Position = projection * view * vec4(pos, 1.0);

    vPos = pos;
    vVec = force / mass / 9.8 / 10;
    vColor = vec3(1.0);
}
)";
const auto fragment_src = R"(
#version 450

in vec3 vColor;

out vec4 fragColor;

void main() {
    fragColor = vec4(vColor, 1.0);
}
)";
const auto vec_geom_src = R"(
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
    vColor = vec3(1, 0, 0);
    EmitVertex();
    gl_Position = projection * view * vec4(vPos[0] + vVec[0], 1.0);
    vColor = vec3(0);
    EmitVertex();
    EndPrimitive();
}
)";
const auto tetra_geom_src = R"(
#version 450
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 6) out;

uniform mat4 projection;
uniform mat4 view;

in vec3 vPos[4];

out vec3 vColor;

void main() {
  vec4[4] vertices;
  for (int i = 0; i < 4; ++i){
    vertices[i] = projection * view * vec4(vPos[i], 1.0);
  }

  vColor = vec3(1.0);

  gl_Position = vertices[0];
  EmitVertex();
  gl_Position = vertices[1];
  EmitVertex();
  gl_Position = vertices[2];
  EmitVertex();
  gl_Position = vertices[3];
  EmitVertex();
  gl_Position = vertices[0];
  EmitVertex();
  gl_Position = vertices[1];
  EmitVertex();
  EndPrimitive();
}
)";
std::unique_ptr<Program> vec_program, tetra_program;
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
    vec_program =
        std::make_unique<Program>(Shader(VERTEX_SHADER, vertex_src),
                                  Shader(FRAGMENT_SHADER, fragment_src),
                                  Shader(GEOMETRY_SHADER, vec_geom_src));

    tetra_program =
        std::make_unique<Program>(Shader(VERTEX_SHADER, vertex_src),
                                  Shader(FRAGMENT_SHADER, fragment_src),
                                  Shader(GEOMETRY_SHADER, tetra_geom_src));
  }
}

void GridRenderer::Update(const std::vector<Particle>& particles) {
  assert(particles.size() == size_);
  vbo->SetSubData(particles);
}

void GridRenderer::Draw(const Camera& camera) {
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
