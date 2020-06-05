#include "Collision.hpp"

#include <array>

using namespace glm;
using namespace std;

void Collision::Compute(RigidBody& rb) const {
  if (const auto rr = Detect(rb)) {
    const auto r = rb.GetOffset(*rr);
    const auto v = rb.GetVelocity(r);
    if (v.y < 0) {
      const auto n = vec3{0, 1, 0};
      const auto theta = rb.GetInertia();
      const auto j_n =
          -(1 + eps_) * dot(v, n) /
          (1 / rb.m_ + dot(n, cross(inverse(theta) * cross(r, n), r)));
      rb.AddImpulse(j_n * n, r);

      if (v.x || v.z) {
        const auto v_t = vec3{v.x, 0, v.z};
        const auto t = normalize(v_t);
        const auto j_t =
            length(v_t) /
            (1 / rb.m_ + dot(t, cross(inverse(theta) * cross(r, t), r)));
        rb.AddImpulse(glm::min(length(j_n) * mu_, j_t) * -t, r);
      }
    }
  }
}

optional<vec3> Collision::Detect(const RigidBody& rb) const {
  const auto half_size = rb.size_ / 2.f;
  const auto x = half_size.x, y = half_size.y, z = half_size.z;
  const auto transform = rb.GetTransform();
  const array<vec3, 8> corners{
      transform * vec4{-x, -y, -z, 1.f}, transform * vec4{x, -y, -z, 1.f},
      transform * vec4{x, y, -z, 1.f},   transform * vec4{-x, y, -z, 1.f},
      transform * vec4{-x, -y, z, 1.f},  transform * vec4{x, -y, z, 1.f},
      transform * vec4{x, y, z, 1.f},    transform * vec4{-x, y, z, 1.f}};

  array<bool, 8> detection{};
  for (size_t i = 0; i < 8; ++i) {
    detection[i] = corners[i].y <= 0;
  }

  vec3 p{0.f};
  size_t n{0};
  for (size_t i = 0; i < 8; ++i) {
    if (detection[i]) {
      p += corners[i];
      n += 1;
    }
  }
  if (n == 0)
    return nullopt;
  else
    return p / float(n);
}
