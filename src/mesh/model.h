#ifndef MODEL_H
#define MODEL_H

#include "json-c/json.h"

typedef struct {
  const char* dirPath;
  char* data;
  struct json_object* json;
} Model;

static void getData(Model* self);

[[nodiscard]]
Model modelCreate(const char* modelDirectory);

void modelDelete(Model* self);

#endif

