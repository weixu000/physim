#pragma once
#include <glm/glm.hpp>
#include <glpp/buffer.hpp>
#include <glpp/vertexarray.hpp>
#include <memory>
#include <vector>

#include "ParticleSystem.hpp"

class Camera;

class SPHRenderer {
public:
  SPHRenderer() = default;

  explicit SPHRenderer(const ParticleSystem& system);

  void Update(const ParticleSystem& system);

  void Draw(const Camera& camera);

private:
  size_t size_;

  std::unique_ptr<glpp::Buffer> vbo_;
  std::unique_ptr<glpp::VertexArray> vao_;
};
