#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;

out MY_VETEX_DATA {
  vec3 vertPos;
  vec3 color;
  vec2 texCoord;
  vec3 normal;
} data_out;

uniform mat4 model;

void main() {
  vec3 vertPos = vec3(model * vec4(pos, 1.f));
  data_out.vertPos = vertPos;
  data_out.normal = normal;
  data_out.color = col;
  data_out.texCoord = mat2(1.f, 0.f, 0.f, -1.f) * tex;
  gl_Position = vec4(vertPos, 1.f);
}

