#pragma once
#include <glm/glm.hpp>
#include <glpp/buffer.hpp>
#include <glpp/vertexarray.hpp>
#include <memory>
#include <vector>

class Camera;
class Particle;

class GridRenderer {
public:
  explicit GridRenderer(const std::vector<Particle>& particles,
                        const std::vector<std::array<glm::uint, 4>>& tetra);

  void Update(const std::vector<Particle>& particles);

  void Draw(const Camera& camera);

private:
  size_t size_, tetra_size_;

  std::unique_ptr<glpp::Buffer> vbo, ebo;
  std::unique_ptr<glpp::VertexArray> vao;
};
