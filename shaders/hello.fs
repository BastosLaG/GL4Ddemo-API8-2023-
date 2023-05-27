#version 330
in float il;
out vec4 fragColor;

void main() {
     fragColor = il * vec4(0.4, 0.4, 0.4, 1.0);
}