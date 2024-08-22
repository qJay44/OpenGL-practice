#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 vertPos;
out vec3 color;
out vec2 texCoord;
out vec3 normal;
out vec4 fragPosLight;

in MY_VETEX_DATA {
  vec3 vertPos;
  vec3 color;
  vec2 texCoord;
  vec3 normal;
} data_in[];

uniform mat4 cam;
uniform mat4 lightProj;

void main() {
  gl_Position = cam * gl_in[0].gl_Position;
  vertPos = data_in[0].vertPos;
  normal = data_in[0].normal;
  color = data_in[0].color;
  texCoord = data_in[0].texCoord;
  fragPosLight = lightProj * gl_in[0].gl_Position;
  EmitVertex();

  gl_Position = cam * gl_in[1].gl_Position;
  vertPos = data_in[1].vertPos;
  normal = data_in[1].normal;
  color = data_in[1].color;
  texCoord = data_in[1].texCoord;
  fragPosLight = lightProj * gl_in[1].gl_Position;
  EmitVertex();

  gl_Position = cam * gl_in[2].gl_Position;
  vertPos = data_in[2].vertPos;
  normal = data_in[2].normal;
  color = data_in[2].color;
  texCoord = data_in[2].texCoord;
  fragPosLight = lightProj * gl_in[2].gl_Position;
  EmitVertex();

  EndPrimitive();
}

