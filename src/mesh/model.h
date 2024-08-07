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
  size_t dataSize;

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
} Model;

[[nodiscard]]
Model modelCreate(const char* modelDirectory);

void modelScale(Model* self, float scale);
void modelDraw(const Model* self, const Camera* camera, GLint shader);
void modelDrawTRC(const Model* self, const Camera* camera, GLint shader, vec3s translation, versors rotation, vec3s scale);
void modelDelete(Model* self);

#endif

