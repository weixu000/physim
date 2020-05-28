#version 450

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view_proj;

out vec3 vPos;
out vec3 vNormal;

void main() {
    gl_Position = view_proj * model * vec4(pos, 1.0);
    vPos = vec3(model * vec4(pos, 1.0));
    vNormal = normal * mat3(inverse(model));
}