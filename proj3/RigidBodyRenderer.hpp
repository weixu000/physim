#pragma once
#include <glm/glm.hpp>
#include <glpp/buffer.hpp>
#include <glpp/vertexarray.hpp>
#include <memory>
#include <vector>

class Camera;
struct Particle;

class RigidBodyRenderer {
public:
  RigidBodyRenderer() = default;

  explicit RigidBodyRenderer(const glm::vec3& size);

  void Update(const glm::mat4& transform);

  void Draw(const Camera& camera);

private:
  std::unique_ptr<glpp::VertexArray> vao;

  glm::vec3 size_{1.f};
  glm::mat4 transform_{1.f};
};
