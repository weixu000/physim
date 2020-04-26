#pragma once

#include <glm/glm.hpp>

class Camera;

class Axes {
public:
  Axes();

  void Draw(const Camera &camera);

private:
  glm::mat4 transform_{1.f};
};
