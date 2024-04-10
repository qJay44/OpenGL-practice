#include "model.h"
#include <string.h>

#define DATA_SIZE 100 * 1000 // 100 KB

static void getData(Model* self) {
  struct json_object* buffers;
  struct json_object* uri;
  json_object_object_get_ex(self->json, "buffers", &buffers);

  struct json_object* buffer0 = json_object_array_get_idx(buffers, 0);
  json_object_object_get_ex(buffer0, "uri", &uri);
  const char* uriStr = json_object_get_string(uri);

  // Folder + file
  rsize_t size = strlen(self->dirPath) + strlen(uriStr) + 1;
  char* dataPath = malloc(size);
  concat(self->dirPath, uriStr, dataPath, size);

  // Read data
  char* buffer = malloc(DATA_SIZE);
  readFile(dataPath, buffer, DATA_SIZE / sizeof(char), false);
  self->data = buffer;
}

Model modelCreate(const char* modelDirectory) {
  static const char* sceneGLTF = "scene.gltf";

  Model model;

  // Folder + file
  rsize_t size = strlen(modelDirectory) + strlen(sceneGLTF) + 1;
  char* gltfPath = malloc(size);
  concat(modelDirectory, sceneGLTF, gltfPath, size);

  // Read .gltf
  char buffer[0xffff];
  readFile(gltfPath, buffer, 0xffff, false);

  model.json = json_tokener_parse(buffer);
  model.dirPath = modelDirectory;
  getData(&model);

  free(gltfPath);
  return model;
}

void modelDelete(Model* self) {
  free(self->data);
}

