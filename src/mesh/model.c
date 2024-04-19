#include "model.h"
#include "object.h"
#include <stdlib.h>
#include <string.h>

#define DATA_SIZE 100 * 1000 // 100 KB

static char* getData(const Model* self) {
  json* buffers;
  json* uri;
  if (!json_object_object_get_ex(self->json, "buffers", &buffers))
    printf("Can't get \"buffers\" from json\n");

  json* buffer0 = json_object_array_get_idx(buffers, 0);
  json_object_object_get_ex(buffer0, "uri", &uri);
  const char* uriStr = json_object_get_string(uri);

  // Folder + file
  rsize_t size = strlen(self->dirPath) + strlen(uriStr) + 1;
  char* dataPath = malloc(size);
  concat(self->dirPath, uriStr, dataPath, size);

  // Read data
  free(dataPath);
  return readFile(dataPath,  false);
}

static float* getFloats(const Model* self, const json* accessor) {
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

  u32 outMaxItems = DEFAULT_BUFFER_ITEMS;
  u32 outIdx = 0;
  float* out = malloc(sizeof(float) * outMaxItems);

  for (u32 i = beginningOfData; i < beginningOfData + lengthOfData; i++) {
    if (outIdx == outMaxItems) {
      arrResizeFloat(&out, outMaxItems * 2);
      outMaxItems *= 2;
    }

    u32 ii = i << 2;
    byte bytes[4] = {self->data[ii], self->data[ii + 1], self->data[ii + 2], self->data[ii + 3]};
    float value;
    memcpy(&value, bytes, sizeof(float));
    out[outIdx++] = value;
  }

  return out;
}

static GLuint* getIndices(const Model* self, const json* accessor) {
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

  u32 outMaxItems = DEFAULT_BUFFER_ITEMS;
  u32 outIdx = 0;
  uint* out = malloc(sizeof(uint) * outMaxItems);

  switch (componentType) {
    // unsigned int
    case 5125:
      for (u32 i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i++) {
        if (outIdx == outMaxItems) {
          arrResizeUint(&out, outMaxItems * 2);
          outMaxItems *= 2;
        }

        u32 ii = i << 2;
        byte bytes[4] = {self->data[ii], self->data[ii + 1], self->data[ii + 2], self->data[ii + 3]};
        unsigned int value;
        memcpy(&value, bytes, sizeof(unsigned int));
        out[outIdx++] = (GLuint)value;
      }
      break;
    // unsigned short
    case 5123:
      for (u32 i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i++) {
        if (outIdx == outMaxItems) {
          arrResizeUint(&out, outMaxItems * 2);
          outMaxItems *= 2;
        }

        u32 ii = i << 1;
        byte bytes[2] = {self->data[ii], self->data[ii + 1]};
        unsigned short value;
        memcpy(&value, bytes, sizeof(unsigned short));
        out[outIdx++] = (GLuint)value;
      }
      break;
    // short
    case 5122:
      for (u32 i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i++) {
        if (outIdx == outMaxItems) {
          arrResizeUint(&out, outMaxItems * 2);
          outMaxItems *= 2;
        }

        u32 ii = i << 1;
        byte bytes[2] = {self->data[ii], self->data[ii + 1]};
        short value;
        memcpy(&value, bytes, sizeof(short));
        out[outIdx++] = (GLuint)value;
      }
      break;
  }

  return (GLuint*)out;
}

static vec2s* getFloatsVec2(const float* vecs, u32 vecsCount) {
  u32 outMaxItems = DEFAULT_BUFFER_ITEMS;
  u32 outIdx = 0;
  vec2s* out = malloc(sizeof(vec2s) * outMaxItems);

  for (int i = 0; i < vecsCount; i++) {
    if (outIdx == outMaxItems) {
      arrResizeVec2s(&out, outMaxItems * 2);
      outMaxItems *= 2;
    }

    u32 ii = i << 1;
    out[outIdx++] = (vec2s){vecs[ii], vecs[ii + 1]};
  }
}

static vec3s* getFloatsVec3(const float* vecs, u32 vecsCount) {
  u32 outMaxItems = DEFAULT_BUFFER_ITEMS;
  u32 outIdx = 0;
  vec3s* out = malloc(sizeof(vec3s) * outMaxItems);

  for (int i = 0; i < vecsCount; i += 3) {
    if (outIdx == outMaxItems) {
      arrResizeVec3s(&out, outMaxItems * 2);
      outMaxItems *= 2;
    }

    out[outIdx++] = (vec3s){vecs[i], vecs[i + 1], vecs[i + 2]};
  }
}

static vec4s* getFloatsVec4(const float* vecs, u32 vecsCount) {
  u32 outMaxItems = DEFAULT_BUFFER_ITEMS;
  u32 outIdx = 0;
  vec4s* out = malloc(sizeof(vec4s) * outMaxItems);

  for (int i = 0; i < vecsCount; i++) {
    if (outIdx == outMaxItems) {
      arrResizeVec4s(&out, outMaxItems * 2);
      outMaxItems *= 2;
    }

    u32 ii = i << 2;
    out[outIdx++] = (vec4s){vecs[ii], vecs[ii + 1], vecs[ii + 2], vecs[ii + 3]};
  }
}

static void assembleVertices(
    vec3s* positions,
    vec3s* normals,
    vec2s* texUVs,
    u32 idx,
    Object* out, u32 outIdx, u32 outIdxLimit
) {
  for (int i = 0; i <= idx; i++) {
    if (outIdx > outIdxLimit) {
      printf("assembleVertice: out index is out of range");
      exit(EXIT_FAILURE);
    }

    const float vertices[10] = {
      positions[i].x, positions[i].y, positions[i].z,
      normals[i].x, normals[i].y, normals[i].z,
      texUVs[i].x, texUVs[i].y
    };

    Object vertex = {
      .vertPtr = vertices,
      .vertSize = 10 * sizeof(float),
      .vertCount = 10
    };

    out[outIdx++] = vertex;
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
  char* buffer = readFile(gltfPath, false);

  model.json = json_tokener_parse(buffer);
  model.dirPath = modelDirectory;
  model.data = getData(&model);

  free(gltfPath);
  free(buffer);
  return model;
}

void modelDelete(Model* self) {
  free(self->data);
}

