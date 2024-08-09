#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 norm;
layout (location = 4) in mat4 instanceMatrix;

out vec3 vertPos;
out vec3 normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 cam;

void main() {
	vertPos = vec3(instanceMatrix * vec4(pos, 1.0f));
	normal = norm;
	color = col;
  texCoord = mat2(1.f, 0.f, 0.f, -1.f) * tex;

	gl_Position = cam * vec4(vertPos, 1.0);
}

