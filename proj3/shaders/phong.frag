#version 450

in vec3 vPos;
in vec3 vNormal;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(vNormal);
    vec3 light = normalize(vec3(-5, 5, -5)-vPos);
    fragColor = vec4(vec3(max(0, dot(light, norm)) + 0.05), 1.0);
}