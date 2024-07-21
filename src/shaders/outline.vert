#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 camMat;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main() {
  vec3 currPos = vec3(model * translation * rotation * scale * vec4(pos, 1.f));
  gl_Position = camMat * vec4(currPos, 1.f);
}

