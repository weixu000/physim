#version 450

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vel;
layout (location = 2) in vec3 force;
layout (location = 3) in float mass;
layout (location = 4) in float density;

uniform mat4 projection;
uniform mat4 view;

out vec3 vColor;

void main() {
    gl_Position = projection * view * vec4(pos, 1.0);
    vColor = vec3(1.0);
}