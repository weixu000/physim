#pragma once
#include <glm/glm.hpp>

class Camera {
public:
  Camera(const glm::vec3 &look_from, const glm::vec3 &look_at, float w,
         float h);

  void Update(float dt);

  void Resize(float w, float h);

  void OnMouseMove(float x, float y);

  void OnMouseButtonPress(float x, float y);

  void OnMouseButtonRelease(float x, float y);

  [[nodiscard]] const glm::mat4 &Projection() const { return projection_; }

  [[nodiscard]] const glm::mat4 &View() const { return view_; }

  [[nodiscard]] glm::vec3 Eye() const { return transform_[3]; }

  [[nodiscard]] float Width() const { return width_; }

  [[nodiscard]] float Height() const { return height_; }

  // Keyboard control
  bool forward_ = false, left_ = false, backward_ = false, right_ = false;

private:
  // Projection parameters
  glm::mat4 projection_;
  float width_, height_;
  float z_near_ = 0.1f, z_far_ = 1000.0f;
  float fovy_ = 60.f;

  // World transform parameters
  glm::mat4 transform_{1.f}, view_{1.f};  // View is inverse of transform
  float yaw_ = 0.f, pitch_ = 0.f;         // Roll is fixed to 0

  // Mouse control
  bool rotating_ = false;
  glm::vec2 previous_{0.f};
};
