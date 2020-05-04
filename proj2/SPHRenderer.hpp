#pragma once
#include <glm/glm.hpp>
#include <glpp/buffer.hpp>
#include <glpp/vertexarray.hpp>
#include <memory>
#include <vector>

#include "ParticleSystem.hpp"
#include "SPHSimulator.hpp"

class Camera;

class SPHRenderer {
public:
  SPHRenderer() = default;

  SPHRenderer(const ParticleSystem& system, const glm::vec3& box,
              const Grid<bool>& grid);

  void Update(const ParticleSystem& system);

  void UpdateMarchingCubes(const Grid<bool>& grid);

  void Draw(const Camera& camera);

private:
  void InitializeParticleVAO(const ParticleSystem& system);
  void InitializeBoxVAO(const glm::vec3& box);
  void InitializeSurfaceVAO(const Grid<bool>& grid);

  size_t size_, triangle_size_;

  std::unique_ptr<glpp::Buffer> vbo_, triangle_vbo_;
  std::unique_ptr<glpp::VertexArray> vao_, triangle_vao_;
  std::unique_ptr<glpp::VertexArray> box_vao_;
};
