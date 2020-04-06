#pragma once
#include <glm/glm.hpp>

class Camera {
public:
  Camera(float w, float h) { Resize(w, h); }

  void Update(float dt);

  void Resize(float w, float h);

  void OnMouseMove(float x, float y);

  void OnMouseButtonPress(float x, float y);

  void OnMouseButtonRelease(float x, float y);

  [[nodiscard]] const glm::mat4 &Projection() const { return projection_; }

  [[nodiscard]] const glm::mat4 &View() const { return view_; }

  [[nodiscard]] const glm::vec3 &Eye() const { return eye_; }

  [[nodiscard]] float Width() const { return width_; }

  [[nodiscard]] float Height() const { return height_; }

  bool forward_ = false, left_ = false, backward_ = false, right_ = false;

private:
  glm::mat4 projection_, view_;
  glm::vec3 eye_;
  float width_, height_;
  float z_near_ = 0.1f, z_far_ = 1000.0f;
  float fovy_ = 60.f;

  glm::mat4 transform_{1.f};

  bool rotating_ = false;
  glm::vec2 previous_{0.f};
  glm::vec2 euler_{0.f};
};
