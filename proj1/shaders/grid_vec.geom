#version 450

layout (points) in;
layout (line_strip, max_vertices = 4) out;

uniform mat4 projection;

in vec3 vAcc[1];
in vec3 vVel[1];

out vec3 aColor;

void main() {
    gl_Position = projection * gl_in[0].gl_Position;
    aColor = vec3(0);
    EmitVertex();
    gl_Position = projection * (gl_in[0].gl_Position + vec4(vAcc[0], 1.0));
    aColor = vec3(1, 0, 0);
    EmitVertex();
    EndPrimitive();

    gl_Position = projection * gl_in[0].gl_Position;
    aColor = vec3(0);
    EmitVertex();
    gl_Position = projection * (gl_in[0].gl_Position + vec4(vVel[0], 1.0));
    aColor = vec3(0, 1, 0);
    EmitVertex();
    EndPrimitive();
}