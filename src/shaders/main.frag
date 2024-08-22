#version 330 core

out vec4 FragColor;

in vec3 vertPos;
in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec4 fragPosLight;

uniform vec3 background;
uniform float near;
uniform float far;
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D shadowMap;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

vec4 pointLight() {
  vec3 lightVec = lightPos - vertPos;
  float dist = length(lightVec);
  float a = 3.f;
  float b = 0.7f;
  float intensity = 1.f / (a * dist * dist + b * dist + 1.f);

  vec3 n = normalize(normal);
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(n, lightDirection), 0.f);
  float ambient = 0.2f;

  float specular = 0.f;
  if (diffuse) {
    float specularLight = 0.5f;
    vec3 viewDirection = normalize(camPos - vertPos);
    vec3 reflectionDirection = reflect(-lightDirection, n);
    vec3 halfwayVec = normalize(viewDirection + lightDirection);

    float specAmount = pow(max(dot(n, halfwayVec), 0.f), 16);
    specular = specAmount * specularLight;
  }

  vec4 diffuse0Col = texture(diffuse0, texCoord) * (diffuse * intensity + ambient);
  float specular0Col = texture(specular0, texCoord).r * specular * intensity;

  return (diffuse0Col + specular0Col) * lightColor;
}

vec4 directionalLight() {
  vec3 n = normalize(normal);
  vec3 lightDirection = normalize(vec3(1.f, 1.f, 0.f));
  float diffuse = max(dot(n, lightDirection), 0.f);
  float ambient = 0.2f;

  float specularLight = 0.5f;
  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, n);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 16);
  float specular = specAmount * specularLight;

  float shadow = 0.f;
  vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
  if (lightCoords.z <= 1.f) {
    lightCoords = (lightCoords + 1.f) * 0.5f;
    float currentDepth = lightCoords.z;
    float bias = max(0.025f * (1.f - dot(normal, lightDirection)), 0.0005f);

		int sampleRadius = 2;
		vec2 pixelSize = 1.f / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++) {
      for(int x = -sampleRadius; x <= sampleRadius; x++) {
        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
        if (currentDepth > closestDepth + bias)
          shadow += 1.f;
      }
		}
		shadow /= pow((sampleRadius * 2 + 1), 2);
  }

  vec4 diffuse0Col = texture(diffuse0, texCoord) * (diffuse * (1.f - shadow) + ambient);
  float specular0Col = texture(specular0, texCoord).r * specular * (1.f - shadow);

  return (diffuse0Col + specular0Col) * lightColor;
}

vec4 spotLight() {
  float outerCone = 0.55f;
  float innerCone = 0.95f;
  float ambient = 0.2f;

  vec3 n = normalize(normal);
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(n, lightDirection), 0.f);

  float specular = 0.f;
  if (diffuse != 0.f) {
    float specularLight = 0.5f;
    vec3 viewDirection = normalize(camPos - vertPos);
    vec3 halfwayVec = normalize(viewDirection + lightDirection);
    float specAmount = pow(max(dot(n, halfwayVec), 0.f), 16);
    specular = specAmount * specularLight;
  }

  float angle = dot(vec3(0.f, -1.f, 0.f), -lightDirection);
  float intensity = clamp((angle - outerCone) / (innerCone - outerCone), 0.f, 1.f);

  float shadow = 0.f;
  vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
  if (lightCoords.z <= 1.f) {
    lightCoords = (lightCoords + 1.f) * 0.5f;
    float currentDepth = lightCoords.z;
    float bias = max(0.00025f * (1.f - dot(normal, lightDirection)), 0.000005f);

		int sampleRadius = 2;
		vec2 pixelSize = 1.f / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++) {
      for(int x = -sampleRadius; x <= sampleRadius; x++) {
        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
        if (currentDepth > closestDepth + bias)
          shadow += 1.f;
      }
		}
		shadow /= pow((sampleRadius * 2 + 1), 2);
  }

  vec4 diffuse0Col = texture(diffuse0, texCoord) * (diffuse * (1.f - shadow) * intensity + ambient);
  float specular0Col = texture(specular0, texCoord).r * specular * (1.f - shadow) * intensity;

  return (diffuse0Col + specular0Col) * lightColor;
}

float linearizeDepth(float depth) {
	return (2.f * near * far) / (far + near - (depth * 2.f - 1.f) * (far - near));
}

float logisticDepth(float depth) {
  float steepness = 0.5f;
  float offset = 5.f;
  float zVal = linearizeDepth(depth);

  return (1.f / (1.f + exp(-steepness * (zVal - offset))));
}

void main() {
  FragColor = spotLight();
}

