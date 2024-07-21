#ifndef MODEL_H
#define MODEL_H

#include "cglm/types-struct.h"
#include "json-c/json.h"

#include "../camera.h"
#include "object.h"
#include "texture.h"

#define MODEL_TEXTURES_LENGTH 0xff

typedef struct json_object json;

typedef struct {
  const char* dirPath;
  json* json;
  byte* data;

  Texture* textures[MODEL_TEXTURES_LENGTH];
  u32 texturesIdx;

  Object* meshes;
  size_t meshesSize;
  u32 meshesIdx;

  vec3s* translationMeshes;
  size_t tmSize;
  u32 tmIdx;

  versors* rotationMeshes;
  size_t rmSize;
  u32 rmIdx;

  vec3s* scaleMeshes;
  size_t smSize;
  u32 smIdx;

  mat4s* matMeshes;
  size_t mmSize;
  u32 mmIdx;

} Model;

[[nodiscard]] static byte* getDataBin(const Model* self);
[[nodiscard]] static float* getFloats(const Model* self, const json* accessor, u32* outCount);
[[nodiscard]] static GLuint* getIndices(const Model* self, const json* accessor, u32* outCount);
[[nodiscard]] static float* assembleVertices(float* positions, float* normals, float* texUVs, u32 count);
static void getTextures(Model* self);

static void loadMesh(Model* self, u32 idxMesh);
static void traverseNode(Model* self, u32 nextNode, mat4 matrix);

[[nodiscard]]
Model modelCreate(const char* modelDirectory);

void modelScale(Model* self, float scale);
void modelDraw(const Model* self, const Camera* camera, GLint shader);
void modelDelete(Model* self);

#endif

