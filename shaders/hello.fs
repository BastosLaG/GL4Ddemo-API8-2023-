#version 330
in float il;
out vec4 fragColor;

void main() {
     fragColor = il * vec4(0.5, 0.5, 1, 1.0);
}