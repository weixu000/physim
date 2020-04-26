#version 450

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;

void main() {
    gl_Position = projection * view * world * vec4(position, 1.0);
}