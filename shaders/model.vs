#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
 
out vec2 vsoTexCoord;
out vec3 vsoNormal;
out vec4 vsoModPosition;

uniform mat4 modelCar;
uniform mat4 projectionView;

void main(void) {
  vsoNormal = (transpose(inverse(modelCar)) * vec4(normal.xyz, 0.0)).xyz;
  vsoModPosition = modelCar * vec4(pos.xyz, 1.0);
  gl_Position = projectionView * modelCar * vec4(pos.xyz, 1.0);
  vsoTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}
