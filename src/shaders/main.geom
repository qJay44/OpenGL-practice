#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 vertPos;
out vec3 color;
out vec2 texCoord;
out vec3 normal;
out vec4 fragPosLight;
out vec3 lightPos;
out vec3 camPos;

in MY_VETEX_DATA {
  vec3 vertPos;
  vec3 color;
  vec2 texCoord;
  vec3 normal;
} data_in[];

uniform mat4 cam;
uniform mat4 model;
uniform mat4 lightProj;
uniform vec3 lightPosUni;
uniform vec3 camPosUni;

void main() {
  vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec2 deltaUV0 = data_in[1].texCoord - data_in[0].texCoord;
  vec2 deltaUV1 = data_in[2].texCoord - data_in[0].texCoord;

  float invDet = 1.f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

  vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
  vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

  mat3 tbn = mat3(
    normalize(vec3(model * vec4(tangent, 0.f))),
    normalize(vec3(model * vec4(bitangent, 0.f))),
    normalize(vec3(model * vec4(cross(edge1, edge0), 0.f)))
  );
  tbn = transpose(tbn);

  gl_Position = cam * gl_in[0].gl_Position;
  vertPos = tbn * data_in[0].vertPos;
  normal = data_in[0].normal;
  color = data_in[0].color;
  texCoord = data_in[0].texCoord;
  fragPosLight = lightProj * gl_in[0].gl_Position;
  lightPos = tbn * lightPosUni;
  camPos = tbn * camPosUni;
  EmitVertex();

  gl_Position = cam * gl_in[1].gl_Position;
  vertPos = tbn * data_in[1].vertPos;
  normal = data_in[1].normal;
  color = data_in[1].color;
  texCoord = data_in[1].texCoord;
  fragPosLight = lightProj * gl_in[1].gl_Position;
  lightPos = tbn * lightPosUni;
  camPos = tbn * camPosUni;
  EmitVertex();

  gl_Position = cam * gl_in[2].gl_Position;
  vertPos = tbn * data_in[2].vertPos;
  normal = data_in[2].normal;
  color = data_in[2].color;
  texCoord = data_in[2].texCoord;
  fragPosLight = lightProj * gl_in[2].gl_Position;
  lightPos = tbn * lightPosUni;
  camPos = tbn * camPosUni;
  EmitVertex();

  EndPrimitive();
}

