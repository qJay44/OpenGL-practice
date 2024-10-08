#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

in vec3 vertPos;
//in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec4 fragPosLight;
in vec3 lightPos;
in vec3 camPos;

uniform vec3 background;
uniform float near;
uniform float far;
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;
uniform sampler2D displacement0;
//uniform sampler2D shadowMap;
//uniform samplerCube shadowCubeMap;
uniform vec4 lightColor;

vec4 pointLight() {
  // intensity
  vec3 lightVec = lightPos - vertPos;
  float dist = length(lightVec);
  float a = 1.f;
  float b = 0.7f;
  float intensity = 1.f / (a * dist * dist + b * dist + 1.f);

  float ambient = 0.05f;

	vec3 viewDirection = normalize(camPos - vertPos);

	// Variables that control parallax occlusion mapping quality
	float heightScale = 0.15f;
	const float minLayers = 8.0f;
  const float maxLayers = 64.0f;
  float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0f, 0.0f, 1.0f), viewDirection)));
	float layerDepth = 1.f / numLayers;
	float currentLayerDepth = 0.f;

	// Remove the z division if you want less aberated results
	vec2 S = viewDirection.xy / viewDirection.z * heightScale;
  vec2 deltaUVs = S / numLayers;

	vec2 UVs = texCoord;
	float currentDepthMapValue = 1.f - texture(displacement0, UVs).r;

	// Loop till the point on the heightmap is "hit"
	while(currentLayerDepth < currentDepthMapValue) {
    UVs -= deltaUVs;
    currentDepthMapValue = 1.f - texture(displacement0, UVs).r;
    currentLayerDepth += layerDepth;
  }

	// Apply Occlusion (interpolation with prev value)
	vec2 prevTexCoords = UVs + deltaUVs;
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = 1.0f - texture(displacement0, prevTexCoords).r - currentLayerDepth + layerDepth;
	float weight = afterDepth / (afterDepth - beforeDepth);
	UVs = prevTexCoords * weight + UVs * (1.0f - weight);

	// Get rid of anything outside the normal range
	if(UVs.x > 1.0 || UVs.y > 1.0 || UVs.x < 0.0 || UVs.y < 0.0)
		discard;

  // diffuse lightning
  vec3 n = normalize(texture(normal0, UVs).xyz * 2.f - 1.f);
  vec3 lightDirection = normalize(lightVec);
  float diffuse = max(dot(n, lightDirection), 0.f);

  // specular lightning
  float specular = 0.f;
  if (diffuse != 0.f) {
    float specularLight = 0.5f;
    vec3 halfwayVec = normalize(viewDirection + lightDirection);
    float specAmount = pow(max(dot(n, halfwayVec), 0.f), 16);
    specular = specAmount * specularLight;
  }

  float shadow = 0.f;
  //vec3 fragToLight = vertPos - lightPos;
  //float currentDepth = length(fragToLight);
  //float bias = max(0.5f * (1.f - dot(n, lightDirection)), 0.0005f);

  //int sampleRadius = 2;
  //float offset = 0.02f;
  //for (int z = -sampleRadius; z <= sampleRadius; z++) {
  //  for (int y = -sampleRadius; y <= sampleRadius; y++) {
  //    for (int x = -sampleRadius; x <= sampleRadius; x++) {
  //      float closestDepth = texture(shadowCubeMap, fragToLight + vec3(x, y, z) * offset).r;
  //      closestDepth *= far;
  //      if (currentDepth > closestDepth + bias)
  //        shadow += 1.f;
  //    }
  //  }
  //}
  //shadow /= pow((sampleRadius * 2 + 1), 3);

  vec4 diffuse0Col = texture(diffuse0, UVs) * (diffuse * (1.f - shadow) * intensity + ambient);
  float specular0Col = texture(specular0, UVs).r * specular * (1.f - shadow) * intensity;

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
  //vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
  //if (lightCoords.z <= 1.f) {
  //  lightCoords = (lightCoords + 1.f) * 0.5f;
  //  float currentDepth = lightCoords.z;
  //  float bias = max(0.025f * (1.f - dot(normal, lightDirection)), 0.0005f);

	//	int sampleRadius = 2;
	//	vec2 pixelSize = 1.f / textureSize(shadowMap, 0);
	//	for(int y = -sampleRadius; y <= sampleRadius; y++) {
  //    for(int x = -sampleRadius; x <= sampleRadius; x++) {
  //      float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
  //      if (currentDepth > closestDepth + bias)
  //        shadow += 1.f;
  //    }
	//	}
	//	shadow /= pow((sampleRadius * 2 + 1), 2);
  //}

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
  if (diffuse) {
    float specularLight = 0.5f;
    vec3 viewDirection = normalize(camPos - vertPos);
    vec3 halfwayVec = normalize(viewDirection + lightDirection);
    float specAmount = pow(max(dot(n, halfwayVec), 0.f), 16);
    specular = specAmount * specularLight;
  }

  float angle = dot(vec3(0.f, -1.f, 0.f), -lightDirection);
  float intensity = clamp((angle - outerCone) / (innerCone - outerCone), 0.f, 1.f);

  float shadow = 0.f;
  //vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
  //if (lightCoords.z <= 1.f) {
  //  lightCoords = (lightCoords + 1.f) * 0.5f;
  //  float currentDepth = lightCoords.z;
  //  float bias = max(0.00025f * (1.f - dot(normal, lightDirection)), 0.000005f);

	//	int sampleRadius = 2;
	//	vec2 pixelSize = 1.f / textureSize(shadowMap, 0);
	//	for(int y = -sampleRadius; y <= sampleRadius; y++) {
  //    for(int x = -sampleRadius; x <= sampleRadius; x++) {
  //      float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
  //      if (currentDepth > closestDepth + bias)
  //        shadow += 1.f;
  //    }
	//	}
	//	shadow /= pow((sampleRadius * 2 + 1), 2);
  //}

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
  FragColor = pointLight();

  if (FragColor.r > 0.05f)
    FragColor.r *= 5.f;

  float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
  BloomColor = brightness > 0.15f ? vec4(FragColor.rgb, 1.f) : vec4(0.f, 0.f, 0.f, 1.f);
}

