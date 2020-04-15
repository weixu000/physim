#version 450

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vel;
layout (location = 2) in vec3 force;
layout (location = 3) in float mass;

uniform mat4 projection;
uniform mat4 view;

out vec3 vPos;
out vec3 vAcc;
out vec3 vVel;
out vec3 vColor;

void main() {
    gl_Position = view * vec4(pos, 1.0);
    vPos = pos;
    vAcc = force / mass / 9.8 / 10;
    vVel = vel / 5;
    vColor = vec3(1.0);
}