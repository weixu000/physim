#include "Particle.hpp"

void Particle::Update(float dt) {
  using namespace glm;
  if (pos.y <= 0) {
    // Collision with y=0
    pos.y = 0;
    if (vel.y < 0) {
      vel.y = -.9f * vel.y;

      const auto normal = max(0.f, -force.y);
      force += vec3(0, normal, 0);

      if (const auto rel_vel = vec3(vel.x, 0, vel.z); length(rel_vel)) {
        force += -mu * normal * normalize(rel_vel);
      }
    }
  }
  const auto a = force / mass;
  vel += a * dt;
  pos += vel * dt;
}
