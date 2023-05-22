#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out float il;

uniform mat4 projectionView, modelCar;

void main() {
     vec3 n = normalize(transpose(inverse(modelCar)) * vec4(normal, 0.0)).xyz;
     vec3 Ld = normalize(vec3(-2.0, -8.0, 5.0));
     il = clamp(dot(n, -Ld), 0.1, 1.0);
     gl_Position = projectionView * modelCar * vec4(pos, 1.0);
}