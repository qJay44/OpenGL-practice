#ifndef MODEL_H
#define MODEL_H

#include "cglm/types-struct.h"
#include "json-c/json.h"
#include "object.h"

typedef struct json_object json;

typedef struct {
  const char* dirPath;
  char* data;
  json* json;
} Model;

static void setData(Model* self);
static void getFloats(const Model* self, const json* accessor, float* out, u32 outIdx, u32 outIdxLimit);
static void getIndices(const Model* self, const json* accessor, GLuint* out, u32 outIdx, u32 outIdxLimit);
static void getFloatsVec2(const float* vecs, u32 vecsCount, vec2s* out, u32 outIdx, u32 outIdxLimit);
static void getFloatsVec3(const float* vecs, u32 vecsCount, vec3s* out, u32 outIdx, u32 outIdxLimit);
static void getFloatsVec4(const float* vecs, u32 vecsCount, vec4s* out, u32 outIdx, u32 outIdxLimit);

static void assembleVertices(
    vec3s* positions,
    vec3s* normals,
    vec2s* texUVs,
    u32 idx,
    Object* out, u32 outIdx, u32 outIdxLimit
);

[[nodiscard]]
Model modelCreate(const char* modelDirectory);

void modelDelete(Model* self);

#endif

