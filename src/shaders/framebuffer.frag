#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform int winWidth;
uniform int winHeight;
uniform float time;

float offsetX = 1.f / winWidth;
float offsetY = 1.f / winWidth;

vec2 offsets[9] = vec2[] (
    vec2(-offsetX, offsetY),  vec2(0.0f, offsetY),  vec2(offsetX, offsetY),
    vec2(-offsetX, 0.0f),     vec2(0.0f, 0.0f),     vec2(offsetX, 0.0f),
    vec2(-offsetX, -offsetY), vec2(0.0f, -offsetY), vec2(offsetX, -offsetY)
);

float kernel[9] = float[] (
    1.f,  1.f, 1.f,
    1.f, -8.f, 1.f,
    1.f,  1.f, 1.f
);

void main() {
  vec3 color = vec3(0.0f);
  for(int i = 0; i < 9; i++)
    color += vec3(texture(screenTexture, texCoord.st + offsets[i])) * kernel[i];
  color.r *= sin(time);
  color.g *= cos(time);
  color.b *= sin(time);
  FragColor = vec4(color, 1.0f);
}

