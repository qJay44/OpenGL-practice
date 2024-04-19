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

static char* getData(const Model* self);
static float* getFloats(const Model* self, const json* accessor);
static GLuint* getIndices(const Model* self, const json* accessor);
static vec2s* getFloatsVec2(const float* vecs, u32 vecsCount);
static vec3s* getFloatsVec3(const float* vecs, u32 vecsCount);
static vec4s* getFloatsVec4(const float* vecs, u32 vecsCount);

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

