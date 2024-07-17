#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;

out vec3 color;
out vec2 texCoord;
out vec3 vertNormal;
out vec3 vertPos;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main() {
  vertPos = vec3(model * translation * -rotation * scale * vec4(pos, 1.f));
  color = col;
  texCoord = mat2(0.f, -1.f, 1.f, 0.f) * tex;
  vertNormal = normal;

  gl_Position = camMatrix * vec4(vertPos, 1.f);
}

