#include "model.h"
#include <stdlib.h>
#include <string.h>

#define DATA_SIZE 100 * 1000 // 100 KB

static void getData(Model* self) {
  json* buffers;
  json* uri;
  json_object_object_get_ex(self->json, "buffers", &buffers);

  json* buffer0 = json_object_array_get_idx(buffers, 0);
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

  free(dataPath);
}

static void getFloats(Model* self, json* accessor, float* out, u32 outIdx, u32 outIdxLimit) {
  u32 buffViewInd = 1;
  u32 accByteOffset = 0;

  json* accBufferView;
  if (json_object_object_get_ex(accessor, "bufferView", &accBufferView))
    buffViewInd = json_object_get_int(accBufferView);

  json* jCount;
  json_object_object_get_ex(accessor, "count", &jCount);
  u32 count = json_object_get_int(jCount);

  json* jAccByteOffset;
  if (json_object_object_get_ex(accessor, "byteOffset", &jAccByteOffset))
    accByteOffset = json_object_get_int(jAccByteOffset);

  json* bufferViews;
  json_object_object_get_ex(self->json, "bufferViews", &bufferViews);
  json* bufferView = json_object_array_get_idx(bufferViews, buffViewInd);

  json* jByteOffset;
  json_object_object_get_ex(bufferView, "byteOffset", &jByteOffset);
  u32 byteOffset = json_object_get_int(jByteOffset);

  json* jType;
  json_object_object_get_ex(accessor, "type", &jType);
  const char* type = json_object_get_string(jType);

  u32 numPerVert;
  if (!strcmp(type, "SCALAR")) numPerVert = 1;
  else if (!strcmp(type, "VEC2")) numPerVert = 2;
  else if (!strcmp(type, "VEC3")) numPerVert = 3;
  else if (!strcmp(type, "VEC4")) numPerVert = 4;
  else {
    printf("type is invalid (not SCALAR, VEC2, VEC3, or VEC4\n)");
    exit(EXIT_FAILURE);
  }

  u32 beginningOfData = byteOffset + accByteOffset;
  u32 lengthOfData = count * 4 * numPerVert;
  for (u32 i = beginningOfData; i < beginningOfData + lengthOfData; i++) {
    if (outIdx > outIdxLimit) {
      printf("getFloats: out array index is out of range");
      exit(EXIT_FAILURE);
    }

    u32 ii = i << 2;
    unsigned char bytes[4] = {self->data[ii], self->data[ii + 1], self->data[ii + 2], self->data[ii + 3]};
    float value;
    memcpy(&value, bytes, sizeof(float));
    out[outIdx++] = value;
  }
}

static void getIndices(Model* self, json* accessor, GLuint* out, u32 outIdx, u32 outIdxLimit) {
  u32 buffViewInd = 0;
  u32 accByteOffset = 0;

  json* accBufferView;
  if (json_object_object_get_ex(accessor, "bufferView", &accBufferView))
    buffViewInd = json_object_get_int(accBufferView);

  json* jCount;
  json_object_object_get_ex(accessor, "count", &jCount);
  u32 count = json_object_get_int(jCount);

  json* jAccByteOffset;
  if (json_object_object_get_ex(accessor, "byteOffset", &jAccByteOffset))
    accByteOffset = json_object_get_int(jAccByteOffset);

  json* jComponentType;
  json_object_object_get_ex(accessor, "componentType", &jComponentType);
  u32 componentType = json_object_get_int(jComponentType);

  json* bufferViews;
  json_object_object_get_ex(self->json, "bufferViews", &bufferViews);
  json* bufferView = json_object_array_get_idx(bufferViews, buffViewInd);

  json* jByteOffset;
  json_object_object_get_ex(bufferView, "byteOffset", &jByteOffset);
  u32 byteOffset = json_object_get_int(jByteOffset);

  u32 beginningOfData = byteOffset + accByteOffset;
  switch (componentType) {
    // unsigned int
    case 5125:
      for (u32 i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i++) {
        if (outIdx > outIdxLimit) {
          printf("getIndices: out array index is out of range");
          exit(EXIT_FAILURE);
        }

        u32 ii = i << 2;
        unsigned char bytes[4] = {self->data[ii], self->data[ii + 1], self->data[ii + 2], self->data[ii + 3]};
        unsigned int value;
        memcpy(&value, bytes, sizeof(unsigned int));
        out[outIdx++] = (GLuint)value;
      }
      break;
    // unsigned short
    case 5123:
      for (u32 i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i++) {
        if (outIdx > outIdxLimit) {
          printf("getIndices: out array index is out of range");
          exit(EXIT_FAILURE);
        }

        u32 ii = i << 1;
        unsigned char bytes[2] = {self->data[ii], self->data[ii + 1]};
        unsigned short value;
        memcpy(&value, bytes, sizeof(unsigned short));
        out[outIdx++] = (GLuint)value;
      }
      break;
    // short
    case 5122:
      for (u32 i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i++) {
        if (outIdx > outIdxLimit) {
          printf("getIndices: out array index is out of range");
          exit(EXIT_FAILURE);
        }

        u32 ii = i << 1;
        unsigned char bytes[2] = {self->data[ii], self->data[ii + 1]};
        short value;
        memcpy(&value, bytes, sizeof(short));
        out[outIdx++] = (GLuint)value;
      }
      break;
  }
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

