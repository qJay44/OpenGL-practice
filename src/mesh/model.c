#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "model.h"
#include "cglm/struct/affine-pre.h"
#include "cglm/struct/mat4.h"
#include "cglm/types.h"
#include "object.h"

#define TEXTURE_NAME_LENGTH 0xff // Maxmimum allowed characters per char* for textures
#define TEXTURE_NAMES_COUNT 20   // Length of array of textures names
#define TEXTURES_NAMES_SIZE (sizeof(char) * TEXTURE_NAME_LENGTH * TEXTURE_NAMES_COUNT)

byte* getData(const Model* self) {
  json* buffers;
  json* uri;
  if (!json_object_object_get_ex(self->json, "buffers", &buffers))
    printf("Can't get \"buffers\" from json\n");

  json* buffer0 = json_object_array_get_idx(buffers, 0);
  if (!json_object_object_get_ex(buffer0, "uri", &uri))
    printf("Can't get \"uri\" from first buffer\n");
  const char* uriStr = json_object_get_string(uri);

  // Folder + file
  rsize_t size = strlen(self->dirPath) + strlen(uriStr) + 1;
  char* dataPath = malloc(size);
  concat(self->dirPath, uriStr, dataPath, size);

  // Read data
  free(dataPath);
  return readFileBytes(dataPath);
}

float* getFloats(const Model* self, const json* accessor, u32* outCount) {
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
    printf("type is invalid (not SCALAR, VEC2, VEC3, or VEC4\n)");
    exit(EXIT_FAILURE);
  }

  u32 beginningOfData = byteOffset + accByteOffset;
  u32 lengthOfData = count * 4 * numPerVert;

  u32 outItemsCount = lengthOfData;
  u32 outIdx = 0;
  float* out = malloc(sizeof(float) * outItemsCount);

  for (u32 i = beginningOfData; i < beginningOfData + lengthOfData; i++) {
    u32 ii = i << 2;
    byte bytes[4] = {self->data[ii], self->data[ii + 1], self->data[ii + 2], self->data[ii + 3]};
    float value;
    memcpy(&value, bytes, sizeof(float));
    out[outIdx++] = value;
  }

  *outCount = outItemsCount;
  return out;
}

GLuint* getIndices(const Model* self, const json* accessor, u32* outCount) {
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

  u32 outItemsCount;
  u32 outIdx = 0;
  GLuint* out;

  switch (componentType) {
    // unsigned int
    case 5125:
      outItemsCount = count * 4;
      out = malloc(sizeof(GLuint) * outItemsCount);
      for (u32 i = 0; i < outItemsCount; i++) {
        u32 ii = beginningOfData + (i << 2);
        byte bytes[4] = {self->data[ii], self->data[ii + 1], self->data[ii + 2], self->data[ii + 3]};
        u32 value;
        memcpy(&value, bytes, sizeof(u32));
        out[outIdx++] = (GLuint)value;
      }
      break;
    // unsigned short
    case 5123:
      outItemsCount = count * 2;
      out = malloc(sizeof(GLuint) * outItemsCount);
      for (u32 i = 0; i < outItemsCount; i++) {
        u32 ii = beginningOfData + (i << 1);
        byte bytes[2] = {self->data[ii], self->data[ii + 1]};
        unsigned short value;
        memcpy(&value, bytes, sizeof(unsigned short));
        out[outIdx++] = (GLuint)value;
      }
      break;
    // short
    case 5122:
      outItemsCount = count * 2;
      out = malloc(sizeof(GLuint) * outItemsCount);
      for (u32 i = 0; i < outItemsCount; i++) {
        u32 ii = beginningOfData + (i << 1);
        byte bytes[2] = {self->data[ii], self->data[ii + 1]};
        short value;
        memcpy(&value, bytes, sizeof(short));
        out[outIdx++] = (GLuint)value;
      }
      break;
    default:
      printf("(getIndices) default case occured\n");
      exit(EXIT_FAILURE);
      break;
  }

  *outCount = outItemsCount;
  return out;
}

vec2s* getFloatsVec2(const float* vecs, u32 vecsCount) {
  u32 outItemsCount = vecsCount;
  vec2s* out = malloc(sizeof(vec2s) * outItemsCount);

  for (int i = 0; i < vecsCount; i++) {
    u32 ii = i << 1;
    out[i] = (vec2s){vecs[ii], vecs[ii + 1]};
  }

  return out;
}

vec3s* getFloatsVec3(const float* vecs, u32 vecsCount) {
  u32 outItemsCount = vecsCount;
  vec3s* out = malloc(sizeof(vec3s) * outItemsCount);

  for (int i = 0; i < vecsCount; i++) {
    u32 ii = i * 3;
    out[i] = (vec3s){vecs[ii], vecs[ii + 1], vecs[ii + 2]};
  }

  return out;
}

vec4s* getFloatsVec4(const float* vecs, u32 vecsCount) {
  u32 outItemsCount = vecsCount;
  vec4s* out = malloc(sizeof(vec4s) * outItemsCount);

  for (int i = 0; i < vecsCount; i++) {
    u32 ii = i << 2;
    out[i] = (vec4s){vecs[ii], vecs[ii + 1], vecs[ii + 2], vecs[ii + 3]};
  }

  return out;
}

Texture* getTextures(Model* self) {
  static u32 unit = 0;

  u32 outItemsCount = 1;
  u32 outIdx = 0;
  Texture* out = malloc(sizeof(Texture) * outItemsCount);

  json* images;
  if (!json_object_object_get_ex(self->json, "images", &images))
    printf("Can't get \"images\" from json\n");

  for (u32 i = 0; i < json_object_array_length(images); i++) {
    json* image = json_object_array_get_idx(images, i);
    json* uri;

    if (!json_object_object_get_ex(image, "uri", &uri))
      printf("Can't get \"uri\" from image\n");

    const char* uriStr = json_object_get_string(uri);

    // Folder + file
    rsize_t size = strlen(self->dirPath) + strlen(uriStr) + 1;
    char* path = malloc(size);
    concat(self->dirPath, uriStr, path, size);

    bool skip = false;
    for (u32 j = 0; j < self->ltnSize / TEXTURES_NAMES_SIZE && self->ltnIdx; j++) {
      if (strcmp(self->loadedTexsNames[j], uriStr)) {
        if (outIdx == outItemsCount) {
          size_t sz = sizeof(Texture) * outItemsCount;
          arrResizeTexture(&out, sz, &sz);
          outItemsCount = sz / sizeof(Texture);
        }
        out[outIdx++] = self->loadedTexs[j];
        skip = true;
        break;
      }
    }

    if (!skip) {
      // if diffuse texture
      if (strstr(uriStr, "baseColor")) {
        // add texture
        Texture tex = textureCreate(path, "diffuse", unit);
        if (outIdx == outItemsCount) {
          size_t sz = sizeof(Texture) * outItemsCount;
          arrResizeTexture(&out, sz, &sz);
          outItemsCount = sz / sizeof(Texture);
        }
        out[outIdx++] = tex;
        cacheTexture(self, tex, (char*)uriStr);

        // if specular texture
      } else if (strstr(uriStr, "metallicRoughness")) {
        Texture tex = textureCreate(path, "specular", unit++);
        if (outIdx == outItemsCount) {
          size_t sz = sizeof(Texture) * outItemsCount;
          arrResizeTexture(&out, sz, &sz);
          outItemsCount = sz / sizeof(Texture);
        }
        out[outIdx++] = tex;
        cacheTexture(self, tex, (char*)uriStr);
      }
    }

    free(path);
  }

  return out;
}

float* assembleVertices(vec3s* positions, u32 positionsCount, vec3s* normals, vec2s* texUVs) {
  float* vertices = malloc(sizeof(float) * 8 * positionsCount);

  for (int i = 0; i <= positionsCount; i++) {
    int ii = i << 3;

    vertices[ii + 0] = positions[i].x;
    vertices[ii + 1] = positions[i].y;
    vertices[ii + 2] = positions[i].z;

    vertices[ii + 3] = normals[i].x;
    vertices[ii + 4] = normals[i].y;
    vertices[ii + 5] = normals[i].z;

    vertices[ii + 6] = texUVs[i].x;
    vertices[ii + 7] = texUVs[i].y;
  }

  return vertices;
}

void cacheTexture(Model* self, Texture tex, char* texName) {
  // Push backs textures (all unique)
  if (self->ltIdx == self->ltSize / sizeof(self->loadedTexs[0])) {
    arrResizeTexture(&self->loadedTexs, self->ltSize, &self->ltSize);
  }
  self->loadedTexs[self->ltIdx++] = tex;

  // Push backs textures names (may repeate)
  if (self->ltnIdx == self->ltnSize / sizeof(self->loadedTexsNames[0])) {
    arrResizeCharPtr(&self->loadedTexsNames, self->ltnSize, &self->ltnSize);
  }
  self->loadedTexsNames[self->ltnIdx++] = (char*)texName;
}

void loadMesh(Model* self, u32 idxMesh) {
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
    printf("Can't get \"uri\" from first buffer\n");

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

  json* jIndices;
  if (!json_object_object_get_ex(primitive0, "indices", &jIndices))
    printf("Can't get \"indices\" from json\n");
  else
    indAccIdx = json_object_get_int(jIndices);

  // ========================================================================= //

  json* accessors;
  if (!json_object_object_get_ex(self->json, "accessors", &accessors))
    printf("Can't get \"accessors\" from json\n");

  u32 posVecsCount;
  float* posVecs = getFloats(self, json_object_array_get_idx(accessors, posAccIdx), &posVecsCount);
  vec3s* positions = getFloatsVec3(posVecs, posVecsCount);

  u32 normalVecsCount;
  float* normalVecs = getFloats(self, json_object_array_get_idx(accessors, normalAccIdx), &normalVecsCount);
  vec3s* normals = getFloatsVec3(normalVecs, normalVecsCount);

  u32 texVecsCount;
  float* texVecs = getFloats(self, json_object_array_get_idx(accessors, texAccIdx), &texVecsCount);
  vec2s* texUVs = getFloatsVec2(texVecs, texVecsCount);

  u32 indicesCount;
  float* vertices = assembleVertices(positions, posVecsCount, normals, texUVs);
  GLuint* indices = getIndices(self, json_object_array_get_idx(accessors, indAccIdx), &indicesCount);
  Texture* textures = getTextures(self);

  // Push back the mesh
  Object mesh = objectCreate(vertices, posVecsCount * 3 * sizeof(float), indices, indicesCount * 3 * sizeof(GLuint), self->shader);
  if (self->meshesIdx == self->meshesSize / sizeof(self->meshes[0])) {
    arrResizeObject(&self->meshes, self->meshesSize, &self->meshesSize);
  }
  self->meshes[self->meshesIdx++] = mesh;

  free(posVecs);
  free(positions);
  free(normalVecs);
  free(normals);
  free(texVecs);
  free(texUVs);
  free(vertices);
  free(indices);
  free(textures);
}

void traverseNode(Model* self, u32 nextNode, mat4 matrix) {
  json* nodes;
  if (!json_object_object_get_ex(self->json, "nodes", &nodes))
    printf("Can't get \"nodes\" from json\n");

  json* node = json_object_array_get_idx(nodes, nextNode);

  // Translation
  vec3 translation = {0.f, 0.f, 0.f};
  json* translationJson;
  if (json_object_object_get_ex(node, "nodes", &translationJson)) {
    assert(json_object_array_length(translationJson) == 3);
    for (int i = 0; i < 3; i++) {
      json* tJson = json_object_array_get_idx(translationJson, i);
      translation[i] = (float)json_object_get_double(tJson);
    }
  }

  // Rotation
  vec4 rotation = {1.f, 0.f, 0.f, 0.f};
  json* rotationJson;
  if (json_object_object_get_ex(node, "rotation", &rotationJson)) {
    for (int i = 0; i < 4; i++) {
      json* rJson = json_object_array_get_idx(rotationJson, i);
      rotation[i] = (float)json_object_get_double(rJson);
    }
  }

  // Scale
  vec3 scale = {1.f, 1.f, 1.f};
  json* scaleJson;
  if (json_object_object_get_ex(node, "scale", &scaleJson)) {
    assert(json_object_array_length(scaleJson) == 3);
    for (int i = 0; i < 3; i++) {
      json* sJson = json_object_array_get_idx(scaleJson, i);
      scale[i] = (float)json_object_get_double(sJson);
    }
  }

  // Matrix
  mat4 matNode = GLM_MAT4_IDENTITY_INIT;
  json* matJson;
  if (json_object_object_get_ex(node, "matrix", &matJson)) {
    assert(json_object_array_length(matJson) == 16);
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        json* mJson = json_object_array_get_idx(matJson, j + i * 4);
        matNode[i][j] = (float)json_object_get_double(mJson);
      }
    }
  }

  mat4 trans = GLM_MAT4_IDENTITY_INIT;
  mat4 sca = GLM_MAT4_IDENTITY_INIT;

  glm_translate(trans, translation);
  glm_scale(sca, scale);

  mat4 matNextNode;
  glm_mat4_mul(matrix, matNode, matNextNode);
  glm_mat4_mul(matNextNode, trans, matNextNode);
  matNextNode[0][0] *= rotation[0];
  matNextNode[1][1] *= rotation[1];
  matNextNode[2][2] *= rotation[2];
  matNextNode[3][3] *= rotation[3];
  glm_mat4_mul(matNextNode, sca, matNextNode);

  json* mesh;
  if (json_object_object_get_ex(node, "mesh", &mesh)) {
    // Push back translation
    if (self->tmIdx == self->tmSize / sizeof(self->translationMeshes[0]))
      arrResizeVec3s(&self->translationMeshes, self->tmSize, &self->tmSize);
    self->translationMeshes[self->tmIdx++] = (vec3s){translation[0], translation[1], translation[2]};

    // Push back rotation
    if (self->rmIdx == self->rmSize / sizeof(self->rotationMeshes[0]))
      arrResizeVec4s(&self->rotationMeshes, self->rmSize, &self->rmSize);
    self->rotationMeshes[self->rmIdx++] = (vec4s){rotation[0], rotation[1], rotation[2], rotation[3]};

    // Push back scale
    if (self->smIdx == self->smSize / sizeof(self->scaleMeshes[0]))
      arrResizeVec3s(&self->scaleMeshes, self->smSize, &self->smSize);
    self->scaleMeshes[self->smIdx++] = (vec3s){scale[0], scale[1], scale[2]};

    // Push back matrices
    if (self->mmIdx == self->mmSize / sizeof(self->matMeshes[0]))
      arrResizeMat4s(&self->matMeshes, self->mmSize, &self->mmSize);
    mat4s m = {
      matNextNode[0][0], matNextNode[0][1], matNextNode[0][2], matNextNode[0][3],
      matNextNode[1][0], matNextNode[1][1], matNextNode[1][2], matNextNode[1][3],
      matNextNode[2][0], matNextNode[2][1], matNextNode[2][2], matNextNode[2][3],
      matNextNode[3][0], matNextNode[3][1], matNextNode[3][2], matNextNode[3][3],
    };
    self->matMeshes[self->mmIdx++] = m;

    loadMesh(self, json_object_get_int(mesh));
  }

  json* children;
  if (json_object_object_get_ex(node, "children", &children)) {
    for (int i = 0; i < json_object_array_length(children); i++) {
      json* child = json_object_array_get_idx(children, i);
      traverseNode(self, json_object_get_int(child), matNextNode);
    }
  }
}

Model modelCreate(const char* modelDirectory, const GLint* shader) {
  static const char* sceneGLTF = "scene.gltf";

  // Folder + file
  rsize_t size = strlen(modelDirectory) + strlen(sceneGLTF) + 1;
  char* gltfPath = malloc(size);
  concat(modelDirectory, sceneGLTF, gltfPath, size);

  // Read .gltf
  char* buffer = readFile(gltfPath, false);

  Model model = {
    .dirPath = modelDirectory,
    .json = json_tokener_parse(buffer),
    .data = getData(&model),
    .shader = shader,
    .ltnIdx = 0,
    .ltnSize = TEXTURES_NAMES_SIZE,
    .loadedTexsNames = malloc(model.ltnSize),
    .ltIdx = 0,
    .ltSize = sizeof(Texture),
    .loadedTexs = malloc(model.ltSize),
    .meshesIdx = 0,
    .meshesSize = sizeof(Object),
    .meshes = malloc(model.meshesSize),
    .tmIdx = 0,
    .tmSize = sizeof(vec3s),
    .translationMeshes = malloc(model.tmSize),
    .rmIdx = 0,
    .rmSize = sizeof(vec4s),
    .rotationMeshes = malloc(model.rmSize),
    .smIdx = 0,
    .smSize = sizeof(vec3s),
    .scaleMeshes = malloc(model.smSize),
    .mmIdx = 0,
    .mmSize = sizeof(mat4s),
    .matMeshes = malloc(model.mmSize),
  };

  mat4 traverseMatInit = GLM_MAT4_IDENTITY_INIT;
  traverseNode(&model, 0, traverseMatInit);

  free(gltfPath);
  free(buffer);

  return model;
}

void modelDraw(const Model* self, const Camera* camera) {
  mat4s matrix = GLMS_MAT4_IDENTITY_INIT;
  vec3s translation = {0.f, 0.f, 0.f};
  vec4s rotation = {1.f, 0.f, 0.f, 0.f};
  vec3s scale = {1.f, 1.f, 1.f};

  for (int i = 0; i < self->meshesIdx; i++)
    objectDraw(&self->meshes[i], camera, matrix, translation, rotation, scale);
}

void modelDelete(Model* self) {
  free(self->data);
  free(self->loadedTexsNames);
  free(self->loadedTexs);
  free(self->meshes);
  free(self->translationMeshes);
  free(self->rotationMeshes);
  free(self->scaleMeshes);
  free(self->matMeshes);
}

