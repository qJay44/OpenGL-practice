#version 460 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 vertNormal;
in vec3 vertPos;

uniform sampler2D tex0;
uniform vec4 lightColor;
uniform vec3 lightPos;

void main() {
  vec3 normal = normalize(vertNormal);
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(normal, lightDirection), 0.f);
  float ambient = 0.2f;

  FragColor = texture(tex0, texCoord) * lightColor * (diffuse + ambient);
}

