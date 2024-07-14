#ifndef MODEL_H
#define MODEL_H

#include "cglm/types-struct.h"
#include "json-c/json.h"

#include "../camera.h"
#include "object.h"
#include "texture.h"

typedef struct json_object json;

typedef struct {
  const char* dirPath;
  json* json;
  byte* data;
  const GLint* shader;

  char** loadedTexsNames;
  size_t ltnSize;
  u32 ltnIdx;
  Texture* loadedTexs;
  size_t ltSize;
  u32 ltIdx;

  Object* meshes;
  size_t meshesSize;
  u32 meshesIdx;

  vec3s* translationMeshes;
  size_t tmSize;
  u32 tmIdx;

  vec4s* rotationMeshes;
  size_t rmSize;
  u32 rmIdx;

  vec3s* scaleMeshes;
  size_t smSize;
  u32 smIdx;

  mat4s* matMeshes;
  size_t mmSize;
  u32 mmIdx;

} Model;

[[nodiscard]] static byte* getData(const Model* self);
[[nodiscard]] static float* getFloats(const Model* self, const json* accessor, u32* outCount);
[[nodiscard]] static GLuint* getIndices(const Model* self, const json* accessor, u32* outCount);
[[nodiscard]] static vec2s* getFloatsVec2(const float* vecs, u32 vecsCount);
[[nodiscard]] static vec3s* getFloatsVec3(const float* vecs, u32 vecsCount);
[[nodiscard]] static vec4s* getFloatsVec4(const float* vecs, u32 vecsCount);
[[nodiscard]] static Texture* getTextures(Model* self, u32* outCount);
[[nodiscard]] static float* assembleVertices(vec3s* positions, u32 positionsCount, vec3s* normals, vec2s* texUVs);

static void cacheTexture(Model* self, Texture tex, const char* texName);
static void loadMesh(Model* self, u32 idxMesh);
static void traverseNode(Model* self, u32 nextNode, mat4 matrix);

[[nodiscard]]
Model modelCreate(const char* modelDirectory, const GLint* shader);

void modelDraw(const Model* self, const Camera* camera);
void modelDelete(Model* self);

#endif

