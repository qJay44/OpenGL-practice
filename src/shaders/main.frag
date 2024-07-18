#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 vertNormal;
in vec3 vertPos;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

vec4 pointLight() {
  vec3 lightVec = lightPos - vertPos;
  float dist = length(lightVec);
  float a = 3.f;
  float b = 0.7f;
  float intensity = 1.f / (a * dist * dist + b * dist + 1.f);

  vec3 normal = normalize(vertNormal);
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(normal, lightDirection), 0.f);
  float ambient = 0.2f;

  float specularLight = 0.5f;
  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, normal);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 16);
  float specular = specAmount * specularLight;

  vec4 diffuse0Col = texture(diffuse0, texCoord) * (diffuse * intensity + ambient);
  float specular0Col = texture(specular0, texCoord).r * specular * intensity;

  return (diffuse0Col + specular0Col) * lightColor;
}

vec4 directionalLight() {
  vec3 normal = normalize(vertNormal);
  vec3 lightDirection = normalize(vec3(1.f, 1.f, 0.f));
  float diffuse = max(dot(normal, lightDirection), 0.f);
  float ambient = 0.2f;

  float specularLight = 0.5f;
  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, normal);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 16);
  float specular = specAmount * specularLight;

  vec4 diffuse0Col = texture(diffuse0, texCoord) * (diffuse + ambient);
  float specular0Col = texture(specular0, texCoord).r * specular;

  return (diffuse0Col + specular0Col) * lightColor;
}

vec4 spotLight() {
  float outerCone = 0.9f;
  float innerCone = 0.95f;

  vec3 normal = normalize(vertNormal);
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(normal, lightDirection), 0.f);
  float ambient = 0.2f;

  float specularLight = 0.5f;
  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, normal);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 16);
  float specular = specAmount * specularLight;

  float angle = dot(vec3(0.f, -1.f, 0.f), -lightDirection);
  float intensity = clamp((angle - outerCone) / (innerCone - outerCone), 0.f, 1.f);

  vec4 diffuse0Col = texture(diffuse0, texCoord) * (diffuse * intensity + ambient);
  float specular0Col = texture(specular0, texCoord).r * specular * intensity;

  return (diffuse0Col + specular0Col) * lightColor;
}

void main() {
  FragColor = directionalLight();
  //FragColor = texture(diffuse0, texCoord);
}

