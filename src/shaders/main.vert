#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 tex;

out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;

void main() {
  gl_Position = camMatrix * vec4(pos, 1.f);
  color = col;
  texCoord = tex;
}

