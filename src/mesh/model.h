#ifndef MODEL_H
#define MODEL_H

#include "cglm/types-struct.h"
#include "json-c/json.h"

typedef struct json_object json;

typedef struct {
  const char* dirPath;
  char* data;
  json* json;
} Model;

static void getData(Model* self);
static void getFloats(Model* self, json* accessor, float* out, u32 outIdx, u32 outIdxLimit);
static void getIndices(Model* self, json* accessor, GLuint* out, u32 outIdx, u32 outIdxLimit);
static void getFloatsVec2(float* vecs, vec2s* out);
static void getFloatsVec3(float* vecs, vec3s* out);
static void getFloatsVec4(float* vecs, vec4s* out);

[[nodiscard]]
Model modelCreate(const char* modelDirectory);

void modelDelete(Model* self);

#endif

