#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in DATA {
  vec3 normal;
  vec3 color;
  vec2 texCoord;
  mat4 projection;
} data_in[];

void main() {
  gl_Position = data_in[0].projection * gl_in[0].gl_Position;
  EmitVertex();
  gl_Position = data_in[0].projection * (gl_in[0].gl_Position + 0.01f * vec4(data_in[0].normal, 0.f));
  EmitVertex();
  EndPrimitive();

  gl_Position = data_in[1].projection * gl_in[1].gl_Position;
  EmitVertex();
  gl_Position = data_in[1].projection * (gl_in[1].gl_Position + 0.01f * vec4(data_in[1].normal, 0.f));
  EmitVertex();
  EndPrimitive();

  gl_Position = data_in[1].projection * gl_in[1].gl_Position;
  EmitVertex();
  gl_Position = data_in[1].projection * (gl_in[1].gl_Position + .01f * vec4(data_in[1].normal, 0.f));
  EmitVertex();
  EndPrimitive();
}

