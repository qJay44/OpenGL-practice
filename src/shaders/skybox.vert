#version 330 core

layout (location = 0) in vec3 pos;

out vec3 texCoord;

uniform mat4 projection;
uniform mat4 view;

void main() {
  vec4 posProj = projection * view * vec4(pos, 1.f);
  gl_Position = vec4(posProj.x, posProj.y, posProj.w, posProj.w);
  texCoord = vec3(pos.x, pos.y, -pos.z);
}

