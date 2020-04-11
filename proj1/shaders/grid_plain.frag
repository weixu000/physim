#version 450

in vec3 aColor;

out vec4 fragColor;

void main() {
    fragColor = vec4(aColor, 1.0);
}
