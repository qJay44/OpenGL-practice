#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;

out DATA {
  vec3 normal;
  vec3 color;
  vec2 texCoord;
  mat4 projection;
  vec4 fragPosLight;
} data_out;

uniform mat4 cam;
uniform mat4 model;
uniform mat4 lightProj;

void main() {
  vec4 currPos = model * vec4(pos, 1.f);
  gl_Position = currPos;
  data_out.normal = normal;
  data_out.color = col;
  data_out.texCoord = mat2(1.f, 0.f, 0.f, -1.f) * tex;
  data_out.projection = cam;
  data_out.fragPosLight = lightProj * currPos;
}

