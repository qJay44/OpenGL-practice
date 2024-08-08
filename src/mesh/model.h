#ifndef MODEL_H
#define MODEL_H

#include "json-c/json.h"

#include "../camera.h"
#include "object.h"

typedef struct json_object json;

typedef struct {
  const char* dirPath;
  json* json;
  byte* data;
  size_t dataSize;

  Object* meshes;
  size_t meshesSize;
  u32 meshesIdx;
} Model;

[[nodiscard]]
Model modelCreate(const char* modelDirectory);

void modelScale(Model* self, float scale);
void modelDraw(const Model* self, const Camera* camera, GLint shader);
void modelDelete(Model* self);

#endif

