#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;

out vec3 color;

uniform mat4 cam;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main() {
  gl_Position = cam * model * translation * rotation * scale * vec4(pos, 1.f);
  color = col;
}

