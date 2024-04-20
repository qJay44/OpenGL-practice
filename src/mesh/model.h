#ifndef MODEL_H
#define MODEL_H

#include "cglm/types-struct.h"
#include "json-c/json.h"
#include "object.h"
#include "texture.h"

typedef struct json_object json;

typedef struct {
  const char* dirPath;
  char* data;
  json* json;

  char** loadedTexNames;
  u32 ltnSize;
  u32 ltnIdx;
  Texture* loadedTexs;
  u32 ltSize;
  u32 ltIdx;
} Model;

[[nodiscard]] static char* getData(const Model* self);
[[nodiscard]] static float* getFloats(const Model* self, const json* accessor);
[[nodiscard]] static GLuint* getIndices(const Model* self, const json* accessor);
[[nodiscard]] static vec2s* getFloatsVec2(const float* vecs, u32 vecsCount);
[[nodiscard]] static vec3s* getFloatsVec3(const float* vecs, u32 vecsCount);
[[nodiscard]] static vec4s* getFloatsVec4(const float* vecs, u32 vecsCount);
[[nodiscard]] static Texture* getTextures(Model* self);
[[nodiscard]] static Object* assembleVertices(vec3s* positions, u32 positionsSize, vec3s* normals, vec2s* texUVs);
static void cacheTexture(Model* self, Texture tex, char* texName);

[[nodiscard]]
Model modelCreate(const char* modelDirectory);

void modelDelete(Model* self);

#endif

