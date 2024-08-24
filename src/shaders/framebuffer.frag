#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform float gamma;

void main() {
  vec3 fragment = texture(screenTexture, texCoord).rgb;
  vec3 bloom = texture(bloomTexture, texCoord).rgb;
  vec3 color = fragment + bloom;

  float exposure = 0.8f;
  vec3 toneMapped = vec3(1.f) - exp(-color * exposure);

  FragColor.rgb = pow(toneMapped, vec3(1.f / gamma));
}

