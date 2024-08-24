#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in MY_VERTEX_DATA {
  vec3 color;
  vec2 texCoord;
  vec3 normal;
} data_in[];

uniform mat4 cam;

const float lengthScale = 0.5f;

void main() {
  gl_Position = cam * gl_in[0].gl_Position;
  EmitVertex();
  gl_Position = cam * (gl_in[0].gl_Position + lengthScale * vec4(data_in[0].normal, 0.f));
  EmitVertex();
  EndPrimitive();

  gl_Position = cam * gl_in[1].gl_Position;
  EmitVertex();
  gl_Position = cam * (gl_in[1].gl_Position + lengthScale * vec4(data_in[1].normal, 0.f));
  EmitVertex();
  EndPrimitive();

  gl_Position = cam * gl_in[2].gl_Position;
  EmitVertex();
  gl_Position = cam * (gl_in[2].gl_Position + lengthScale * vec4(data_in[2].normal, 0.f));
  EmitVertex();
  EndPrimitive();
}

