#include "Camera.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

void Camera::Update(float dt) {
  const auto movement =
      vec3(transform_[2] * (float(backward_) - float(forward_)) +
           transform_[0] * (float(right_) - float(left_)));
  transform_ = glm::translate(movement * dt) * transform_;

  view_ = glm::inverse(transform_);
  eye_ = transform_[3];
}

void Camera::Resize(float w, float h) {
  width_ = w;
  height_ = h;
  projection_ =
      glm::perspective(glm::radians(fovy_), width_ / height_, z_near_, z_far_);
}

void Camera::OnMouseMove(float x, float y) {
  if (rotating_) {
    const auto current = vec2(x, y);
    const auto delta = (current - previous_) * 1E-3f;
    euler_ = mod(euler_ + delta, 2 * pi<float>());
    transform_ =
        translate(vec3(transform_[3])) * eulerAngleYX(euler_.x, euler_.y);
    previous_ = current;
  }
}

void Camera::OnMouseButtonPress(float x, float y) {
  rotating_ = true;
  previous_ = glm::vec2(x, y);
}

void Camera::OnMouseButtonRelease(float x, float y) { rotating_ = false; }
