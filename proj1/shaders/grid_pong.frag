#version 450

in vec3 aPos;
flat in vec3 aNormal;
flat in vec3 aColor;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(aNormal);
    vec3 light = normalize(vec3(-5, 5, -5)-aPos);
    fragColor = vec4(aColor * (max(0, dot(light, norm)) + 0.05), 1.0);
}