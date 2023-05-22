#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out float il;
uniform mat4 projectionView, modelRoad;

void main() {
     vec3 Ld = normalize(vec3(-2.0, -8.0, 5.0));
     vec3 n = normalize(transpose(inverse(modelRoad)) * vec4(normal, 0.0)).xyz;
     il = clamp(dot(n, -Ld), 0.1, 1.0);
     gl_Position = projectionView * modelRoad * vec4(pos, 1.0);
}