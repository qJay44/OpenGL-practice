#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

out vec2 texCoord;

void main() {
  gl_Position = vec4(pos.x, pos.y, 0.f, 1.f);
  texCoord = tex;
}

