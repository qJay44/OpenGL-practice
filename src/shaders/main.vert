#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;

out vec3 color;
out vec2 texCoord;
out vec3 vertNormal;
out vec3 vertPos;

uniform mat4 matModel;
uniform mat4 matCam;

void main() {
  vertPos = vec3(matModel * vec4(pos, 1.f));

  gl_Position = matCam * vec4(vertPos, 1.f);
  color = col;
  texCoord = tex;
  vertNormal = normal;
}

