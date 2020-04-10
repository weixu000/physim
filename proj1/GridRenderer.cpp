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
    gl_Position = view * vec4(pos, 1.0);
    vPos = pos;
    vVec = force / mass / 9.8 / 10;
    vColor = vec3(1.0);
}
)";
const auto vec_geom_src = R"(
#version 450
layout (points) in;
layout (line_strip, max_vertices = 2) out;

uniform mat4 projection;

in vec3 vVec[1];

out vec3 aColor;

void main() {
    gl_Position = projection * gl_in[0].gl_Position;
    aColor = vec3(0);
    EmitVertex();
    gl_Position = projection * (gl_in[0].gl_Position + vec4(vVec[0], 1.0));
    aColor = vec3(1, 0, 0);
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

out vec3 aPos;
out vec3 aNormal;
out vec3 aColor;

void main() {
  vec4[4] vertices;
  for (int i = 0; i < 4; ++i){
    vertices[i] = projection * gl_in[i].gl_Position;
  }

  aColor = vec3(1.);

  gl_Position = vertices[0];
  aPos = vPos[0];
  EmitVertex();
  gl_Position = vertices[1];
  aPos = vPos[1];
  EmitVertex();
  gl_Position = vertices[2];
  aPos = vPos[2];
//  aColor = vec3(.2, .73, .93);
  aNormal = cross(vPos[0] - vPos[2], vPos[1] - vPos[2]);
  EmitVertex();
  gl_Position = vertices[3];
  aPos = vPos[3];
//  aColor = vec3(.8, .13, .4);
  aNormal = cross(vPos[2] - vPos[3], vPos[1] - vPos[3]);
  EmitVertex();
  gl_Position = vertices[0];
  aPos = vPos[0];
//  aColor = vec3(.2, .2, .4);
  aNormal = cross(vPos[2] - vPos[0], vPos[3] - vPos[0]);
  EmitVertex();
  gl_Position = vertices[1];
  aPos = vPos[1];
//  aColor = vec3(.6, .93, .13);
  aNormal = cross(vPos[0] - vPos[1], vPos[3] - vPos[1]);
  EmitVertex();
  EndPrimitive();
}
)";
const auto plain_frag_src = R"(
#version 450

in vec3 aColor;

out vec4 fragColor;

void main() {
    fragColor = vec4(aColor, 1.0);
}
)";
const auto phong_frag_src = R"(
#version 450

in vec3 aPos;
flat in vec3 aNormal;
flat in vec3 aColor;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(aNormal);
    vec3 light = normalize(vec3(-5, 5, -5)-aPos);
    fragColor = vec4(aColor * (max(0, dot(light, norm)) + 0.05), 1.0);
}
)";

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
    vec_program =
        std::make_unique<Program>(Shader(VERTEX_SHADER, vertex_src),
                                  Shader(FRAGMENT_SHADER, plain_frag_src),
                                  Shader(GEOMETRY_SHADER, vec_geom_src));

    tetra_program =
        std::make_unique<Program>(Shader(VERTEX_SHADER, vertex_src),
                                  Shader(FRAGMENT_SHADER, plain_frag_src),
                                  Shader(GEOMETRY_SHADER, tetra_geom_src));
    surf_program =
        std::make_unique<Program>(Shader(VERTEX_SHADER, vertex_src),
                                  Shader(FRAGMENT_SHADER, phong_frag_src),
                                  Shader(GEOMETRY_SHADER, tetra_geom_src));
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
