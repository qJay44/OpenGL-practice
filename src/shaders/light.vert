#version 460 core

layout (location = 0) in vec3 pos;

uniform mat4 matModel;
uniform mat4 matCam;

void main() {
  gl_Position = matCam * matModel * vec4(pos, 1.f);
}

