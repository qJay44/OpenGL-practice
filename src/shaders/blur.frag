#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D screenTexture;
uniform bool horizontal;

// How far from the center to take samples from the fragment you are currently on
const int radius = 6;
// Keep it between 1.f and 2.f (the higher this is the further the blur reaches)
float spreadBlur = 2.f;
float weights[radius];

void main() {
  // Calculate the weights using the Gaussian equation
  float x = 0.f;
  for (int i = 0; i < radius; i++) {
    // Decides the distance between each sample on the Gaussian function
    if (spreadBlur <= 2.f)
      x += 3.f / radius;
    else
      x += 6.f / radius;

    weights[i] = exp(-0.5f * pow(x / spreadBlur, 2.f)) / (spreadBlur * sqrt(2.f * 3.14159265f));
  }

  vec2 texOffset = 1.0f / textureSize(screenTexture, 0);
  vec3 result = texture(screenTexture, texCoord).rgb * weights[0];

  // Calculate horizontal blur
  if(horizontal) {
    for(int i = 1; i < radius; i++) {
      // Take into account pixels to the right
      result += texture(screenTexture, texCoord + vec2(texOffset.x * i, 0.f)).rgb * weights[i];
      // Take into account pixels on the left
      result += texture(screenTexture, texCoord - vec2(texOffset.x * i, 0.f)).rgb * weights[i];
    }
  }
  // Calculate vertical blur
  else {
    for(int i = 1; i < radius; i++) {
      // Take into account pixels above
      result += texture(screenTexture, texCoord + vec2(0.f, texOffset.y * i)).rgb * weights[i];
      // Take into account pixels below
      result += texture(screenTexture, texCoord - vec2(0.f, texOffset.y * i)).rgb * weights[i];
    }
  }
  FragColor = vec4(result, 1.0f);
}

