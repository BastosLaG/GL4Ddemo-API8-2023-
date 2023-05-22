#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;


uniform mat4 projectionView, modelRoad;

out vec2 vsoTexCoord;

void main() {
    gl_Position = projectionView * modelRoad * vec4(pos, 1.);
    vsoTexCoord = vec2(texCoord.x, 1 - texCoord.y);
}
