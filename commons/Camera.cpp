#include "Camera.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

Camera::Camera(const glm::vec3& look_from, const glm::vec3& look_at, float w,
               float h) {
  const auto v = normalize(look_from - look_at);  // Look at -z direction
  yaw_ = atan2(v.x, v.z);
  pitch_ = -asin(v.y);

  transform_ = translate(look_from) * yawPitchRoll(yaw_, pitch_, 0.f);
  view_ = inverse(transform_);

  Resize(w, h);
}

void Camera::Update(float dt) {
  const auto movement =
      vec3(transform_[2] * (float(backward_) - float(forward_)) +
           transform_[0] * (float(right_) - float(left_)));
  transform_ = translate(movement * dt * 5.f) * transform_;
  view_ = inverse(transform_);
}

void Camera::Resize(float w, float h) {
  width_ = w;
  height_ = h;
  projection_ = perspective(radians(fovy_), width_ / height_, z_near_, z_far_);
}

void Camera::OnMouseMove(float x, float y) {
  if (rotating_) {
    const auto current = vec2(x / width_, y / height_);
    const auto delta = (current - previous_) * 1.f;
    pitch_ += delta.y;
    yaw_ += delta.x;
    transform_ = translate(Eye()) * yawPitchRoll(yaw_, pitch_, 0.f);
    previous_ = current;
  }
}

void Camera::OnMouseButtonPress(float x, float y) {
  rotating_ = true;
  previous_ = vec2(x / width_, y / height_);
}

void Camera::OnMouseButtonRelease() { rotating_ = false; }
