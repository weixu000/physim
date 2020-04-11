#version 450
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 6) out;

uniform mat4 projection;
uniform mat4 view;

in vec3 vPos[4];

out vec3 aPos;
out vec3 aNormal;
out vec3 aColor;

void main() {
    vec4[4] vertices;
    for (int i = 0; i < 4; ++i){
        vertices[i] = projection * gl_in[i].gl_Position;
    }

    aColor = vec3(1.);

    gl_Position = vertices[0];
    aPos = vPos[0];
    EmitVertex();
    gl_Position = vertices[1];
    aPos = vPos[1];
    EmitVertex();
    gl_Position = vertices[2];
    aPos = vPos[2];
    //  aColor = vec3(.2, .73, .93);
    aNormal = cross(vPos[0] - vPos[2], vPos[1] - vPos[2]);
    EmitVertex();
    gl_Position = vertices[3];
    aPos = vPos[3];
    //  aColor = vec3(.8, .13, .4);
    aNormal = cross(vPos[2] - vPos[3], vPos[1] - vPos[3]);
    EmitVertex();
    gl_Position = vertices[0];
    aPos = vPos[0];
    //  aColor = vec3(.2, .2, .4);
    aNormal = cross(vPos[2] - vPos[0], vPos[3] - vPos[0]);
    EmitVertex();
    gl_Position = vertices[1];
    aPos = vPos[1];
    //  aColor = vec3(.6, .93, .13);
    aNormal = cross(vPos[0] - vPos[1], vPos[3] - vPos[1]);
    EmitVertex();
    EndPrimitive();
}