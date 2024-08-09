#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cglm/affine.h"
#include "cglm/mat4.h"
#include "cglm/struct/mat4.h"
#include "cglm/types.h"
#include "cglm/quat.h"

#include "model.h"
#include "object.h"

static byte* getDataBin(const Model* self, size_t* outSize) {
  json* buffers;
  json* uri;
  if (!json_object_object_get_ex(self->json, "buffers", &buffers))
    printf("Can't get \"buffers\" from json\n");

  json* buffer0 = json_object_array_get_idx(buffers, 0);
  if (!json_object_object_get_ex(buffer0, "uri", &uri))
    printf("Can't get \"uri\" from first buffer\n");
  const char* uriStr = json_object_get_string(uri);

  // Folder + / + file
  char path[strlen(self->dirPath) + strlen(uriStr) + 1];
  sprintf(path, "%s/%s", self->dirPath, uriStr);

  return readFileBytes(path, outSize);
}

static float* getFloats(const Model* self, const json* accessor, u32* outCount) {
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
  if      (!strcmp(type, "SCALAR")) numPerVert = 1;
  else if (!strcmp(type, "VEC2"))   numPerVert = 2;
  else if (!strcmp(type, "VEC3"))   numPerVert = 3;
  else if (!strcmp(type, "VEC4"))   numPerVert = 4;
  else {
    printf("Unhandled vertices type (%s)\n", type);
    exit(EXIT_FAILURE);
  }

  u32 beginningOfData = byteOffset + accByteOffset;
  u32 lengthOfData = count * numPerVert;

  float* out = malloc(sizeof(float) * lengthOfData);
  for (u32 i = 0; i < lengthOfData; i++) {
    u32 ii = beginningOfData + (i << 2);
    assert(ii + 3 < self->dataSize / sizeof(self->data[0]));
    byte bytes[4] = {self->data[ii], self->data[ii + 1], self->data[ii + 2], self->data[ii + 3]};
    float value;
    memcpy(&value, bytes, sizeof(float));
    out[i] = value;
  }

  *outCount = count;
  return out;
}

static GLuint* getIndices(const Model* self, const json* accessor, u32* outCount) {
  u32 buffViewInd = 0;
  u32 accByteOffset = 0;
  u32 byteOffset = 0;

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
  if (json_object_object_get_ex(bufferView, "byteOffset", &jByteOffset))
    byteOffset = json_object_get_int(jByteOffset);

  u32 beginningOfData = byteOffset + accByteOffset;
  GLuint* out = malloc(sizeof(GLuint) * count);

  switch (componentType) {
    // unsigned int
    case 5125:
      for (u32 i = 0; i < count; i++) {
        u32 ii = beginningOfData + (i << 2);
        assert(ii + 3 < self->dataSize / sizeof(self->data[0]));
        byte bytes[4] = {self->data[ii], self->data[ii + 1], self->data[ii + 2], self->data[ii + 3]};
        u32 value;
        memcpy(&value, bytes, sizeof(u32));
        out[i] = (GLuint)value;
      }
      break;
    // unsigned short
    case 5123:
      for (u32 i = 0; i < count; i++) {
        u32 ii = beginningOfData + (i << 1);
        assert(ii + 1 < self->dataSize / sizeof(self->data[0]));
        byte bytes[2] = {self->data[ii], self->data[ii + 1]};
        unsigned short value;
        memcpy(&value, bytes, sizeof(unsigned short));
        out[i] = (GLuint)value;
      }
      break;
    // short
    case 5122:
      for (u32 i = 0; i < count; i++) {
        u32 ii = beginningOfData + (i << 1);
        assert(ii + 1 < self->dataSize / sizeof(self->data[0]));
        byte bytes[2] = {self->data[ii], self->data[ii + 1]};
        short value;
        memcpy(&value, bytes, sizeof(short));
        out[i] = (GLuint)value;
      }
      break;
    default:
      printf("Unhandled component type\n");
      exit(EXIT_FAILURE);
      break;
  }

  *outCount = count;
  return out;
}

static float* assembleVertices(float* positions, float* normals, float* texUVs, u32 count) {
  float* vertices = malloc(sizeof(float) * OBJECT_VERTEX_ATTRIBUTES * count);

  /* Assuming that:
     * positions are vec3,
     * normals are vec3,
     * texUVs are vec2
  */
  // NOTE: Must always be in this order: positions (3), colors (3), texture coords (2), normals (3)
  for (int i = 0; i < count; i++) {
    int i11 = i * OBJECT_VERTEX_ATTRIBUTES;
    int i3 = i * 3;
    int i2 = i * 2;

    assert(i11 < count * OBJECT_VERTEX_ATTRIBUTES);
    assert(i3 < count * 3);
    assert(i2 < count * 2);

    vertices[i11 + 0] = positions[i3 + 0];
    vertices[i11 + 1] = positions[i3 + 1];
    vertices[i11 + 2] = positions[i3 + 2];

    // Color
    vertices[i11 + 3] = 1.f;
    vertices[i11 + 4] = 1.f;
    vertices[i11 + 5] = 1.f;

    vertices[i11 + 6] = texUVs[i2 + 0];
    vertices[i11 + 7] = texUVs[i2 + 1];

    vertices[i11 + 8]  = normals[i3 + 0];
    vertices[i11 + 9]  = normals[i3 + 1];
    vertices[i11 + 10] = normals[i3 + 2];
  }

  return vertices;
}

static void loadTextures(Model* self, Object* obj) {
  json* images;
  if (!json_object_object_get_ex(self->json, "images", &images))
    printf("Can't get \"images\" from json\n");

  for (u32 i = 0; i < json_object_array_length(images); i++) {
    json* image = json_object_array_get_idx(images, i);
    json* uri;

    if (!json_object_object_get_ex(image, "uri", &uri))
      printf("Can't get \"uri\" from image\n");

    const char* uriStr = json_object_get_string(uri);

    // Folder + / + file
    char path[strlen(self->dirPath) + strlen(uriStr) + 1];
    sprintf(path, "%s/%s", self->dirPath, uriStr);

    objectAddTexture(obj, uriStr, path);
  }
}

static void loadMesh(Model* self, u32 idxMesh, mat4s mat) {
  u32 posAccIdx;
  u32 normalAccIdx;
  u32 texAccIdx;
  u32 indAccIdx;

  // ========== JSON["meshes"][idxMesh]["primitives"][0] ========== //

  json* meshes;
  if (!json_object_object_get_ex(self->json, "meshes", &meshes))
    printf("Can't get \"meshes\" from json\n");

  json* jMesh = json_object_array_get_idx(meshes, idxMesh);
  json* primitives;
  if (!json_object_object_get_ex(jMesh, "primitives", &primitives))
    printf("Can't get \"primitives\" from json\n");

  json* primitive0 = json_object_array_get_idx(primitives, 0);

  // ========== JSON["meshes"][idxMesh]["primitives"][0]["attributes"] ========== //

  json* attributes;
  if (!json_object_object_get_ex(primitive0, "attributes", &attributes))
    printf("Can't get \"attributes\" from json\n");

  // ========== JSON["meshes"][idxMesh]["primitives"][0]["attributes"]["POSITION"] ========== //

  json* position;
  if (!json_object_object_get_ex(attributes, "POSITION", &position))
    printf("Can't get \"POSITION\" from json\n");
  else
    posAccIdx = json_object_get_int(position);

  // ========== JSON["meshes"][idxMesh]["primitives"][0]["attributes"]["NORMAL"] ========== //

  json* normal;
  if (!json_object_object_get_ex(attributes, "NORMAL", &normal))
    printf("Can't get \"NORMAL\" from json\n");
  else
    normalAccIdx = json_object_get_int(normal);

  // ========== JSON["meshes"][idxMesh]["primitives"][0]["attributes"]["TEXCOORD_0"] ========== //

  json* texCoord0;
  if (!json_object_object_get_ex(attributes, "TEXCOORD_0", &texCoord0))
    printf("Can't get \"TEXCOORD_0\" from json\n");
  else
    texAccIdx = json_object_get_int(texCoord0);

  // ========== JSON["meshes"][idxMesh]["primitives"][0]["indices"] ========== //

  json* indicesJson;
  if (!json_object_object_get_ex(primitive0, "indices", &indicesJson))
    printf("Can't get \"indices\" from json\n");
  else
    indAccIdx = json_object_get_int(indicesJson);

  // ========================================================================= //

  json* accessors;
  if (!json_object_object_get_ex(self->json, "accessors", &accessors))
    printf("Can't get \"accessors\" from json\n");

  u32 posVecsCount;
  float* positions = getFloats(self, json_object_array_get_idx(accessors, posAccIdx), &posVecsCount);

  u32 normalVecsCount;
  float* normals = getFloats(self, json_object_array_get_idx(accessors, normalAccIdx), &normalVecsCount);

  u32 texVecsCount;
  float* texUVs = getFloats(self, json_object_array_get_idx(accessors, texAccIdx), &texVecsCount);

  assert(posVecsCount == normalVecsCount && normalVecsCount == texVecsCount);
  float* vertices = assembleVertices(positions, normals, texUVs, posVecsCount);

  u32 indicesCount;
  GLuint* indices = getIndices(self, json_object_array_get_idx(accessors, indAccIdx), &indicesCount);

  Object mesh = objectCreate(vertices, sizeof(float) * posVecsCount * OBJECT_VERTEX_ATTRIBUTES, indices, indicesCount * sizeof(GLuint));
  loadTextures(self, &mesh);
  mesh.mat = mat;

  // Push back the mesh
  if (self->meshesIdx == self->meshesSize / sizeof(self->meshes[0]))
    arrResizeObject(&self->meshes, self->meshesSize, &self->meshesSize);
  self->meshes[self->meshesIdx++] = mesh;

  free(positions);
  free(normals);
  free(texUVs);

  // Freeing, because object copies vertices and indices (they can be constructed from stack/heap)
  free(vertices);
  free(indices);
}

static void traverseNode(Model* self, u32 nextNode, mat4s matrix) {
  json* nodes;
  if (!json_object_object_get_ex(self->json, "nodes", &nodes))
    printf("Can't get \"nodes\" from json\n");

  json* node = json_object_array_get_idx(nodes, nextNode);

  // Translation
  vec3s translation = {0.f, 0.f, 0.f};
  json* translationJson;
  if (json_object_object_get_ex(node, "nodes", &translationJson)) {
    assert(json_object_array_length(translationJson) == 3);
    for (int i = 0; i < 3; i++) {
      json* tJson = json_object_array_get_idx(translationJson, i);
      translation.raw[i] = (float)json_object_get_double(tJson);
    }
  }

  // Rotation
  mat4s rotation = GLMS_MAT4_IDENTITY_INIT; {
    versor rot = {0.f, 0.f, 0.f, 1.f};
    json* rotationJson;
    if (json_object_object_get_ex(node, "rotation", &rotationJson)) {
      assert(json_object_array_length(rotationJson) == 4);
      for (int i = 0; i < 4; i++) {
        json* rJson = json_object_array_get_idx(rotationJson, i);
        rot[i] = (float)json_object_get_double(rJson);
      }
    }
    glm_quat_mat4(rot, rotation.raw);
  }

  // Scale
  vec3s scale = {1.f, 1.f, 1.f};
  json* scaleJson;
  if (json_object_object_get_ex(node, "scale", &scaleJson)) {
    assert(json_object_array_length(scaleJson) == 3);
    for (int i = 0; i < 3; i++) {
      json* sJson = json_object_array_get_idx(scaleJson, i);
      scale.raw[i] = (float)json_object_get_double(sJson);
    }
  }

  // Matrix
  mat4s matNode = GLMS_MAT4_IDENTITY_INIT;
  json* matJson;
  if (json_object_object_get_ex(node, "matrix", &matJson)) {
    assert(json_object_array_length(matJson) == 16);
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        json* mJson = json_object_array_get_idx(matJson, j + i * 4);
        matNode.raw[i][j] = (float)json_object_get_double(mJson);
      }
    }
  }

  mat4s matNextNode = glms_mat4_mul(matrix, matNode);
  glm_translate(matNextNode.raw, translation.raw);
  glm_mat4_mul(matNextNode.raw, rotation.raw, matNextNode.raw);
  glm_scale(matNextNode.raw, scale.raw);

  json* mesh;
  if (json_object_object_get_ex(node, "mesh", &mesh))
    loadMesh(self, json_object_get_int(mesh), matNextNode);

  json* children;
  if (json_object_object_get_ex(node, "children", &children)) {
    for (int i = 0; i < json_object_array_length(children); i++) {
      json* child = json_object_array_get_idx(children, i);
      traverseNode(self, json_object_get_int(child), matNextNode);
    }
  }
}

Model modelCreate(const char* modelDirectory) {
  static const char* sceneGLTF = "scene.gltf";

  // Folder + / + file
  char gltfPath[strlen(modelDirectory) + strlen(sceneGLTF) + 1];
  sprintf(gltfPath, "%s/%s", modelDirectory, sceneGLTF);

  // Read .gltf
  char* buffer = readFile(gltfPath, false);

  Model model;
  model.dirPath = modelDirectory;
  model.json = json_tokener_parse(buffer);
  model.data = getDataBin(&model, &model.dataSize);
  model.meshesIdx = 0;
  model.meshesSize = sizeof(Object);
  model.meshes = malloc(model.meshesSize);

  traverseNode(&model, 0, (mat4s)GLMS_MAT4_IDENTITY_INIT);

  free(buffer);

  return model;
}

void modelScale(Model* self, float scale) {
  for (int i = 0; i < self->meshesIdx; i++)
    glm_scale(self->meshes[i].mat.raw, (vec3){scale, scale, scale});
}

void modelDraw(const Model* self, const Camera* camera, GLint shader) {
  for (int i = 0; i < self->meshesIdx; i++)
    objectDraw(&self->meshes[i], camera, shader);
}

void modelDelete(Model* self) {
  free(self->data);
  json_object_put(self->json);

  for (int i = 0; i < self->meshesIdx; i++)
    objectDelete(&self->meshes[i]);

  free(self->meshes);
}

