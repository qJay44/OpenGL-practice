#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "model.h"
#include "cglm/mat4.h"
#include "cglm/struct/affine-pre.h"
#include "cglm/types.h"
#include "cglm/quat.h"
#include "object.h"

#define MODEL_CACHED_TEXTURES_LENGTH 20

byte* getDataBin(const Model* self) {
  json* buffers;
  json* uri;
  if (!json_object_object_get_ex(self->json, "buffers", &buffers))
    printf("Can't get \"buffers\" from json\n");

  json* buffer0 = json_object_array_get_idx(buffers, 0);
  if (!json_object_object_get_ex(buffer0, "uri", &uri))
    printf("Can't get \"uri\" from first buffer\n");
  const char* uriStr = json_object_get_string(uri);

  // Folder + file
  u32 pathLength = strlen(self->dirPath) + strlen(uriStr) + 1;
  char path[pathLength];
  concat(self->dirPath, uriStr, path, sizeof(char) * pathLength);

  return readFileBytes(path);
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
    printf("Unhandled vertices type (not SCALAR, VEC2, VEC3, or VEC4\n)");
    exit(EXIT_FAILURE);
  }

  u32 beginningOfData = byteOffset + accByteOffset;
  u32 lengthCommon = count * 4;
  u32 lengthOfData = lengthCommon * numPerVert;

  float* out = malloc(sizeof(float) * lengthOfData);

  for (u32 i = 0; i < lengthOfData; i++) {
    u32 ii = beginningOfData + (i << 2);
    byte bytes[4] = {self->data[ii], self->data[ii + 1], self->data[ii + 2], self->data[ii + 3]};
    float value;
    memcpy(&value, bytes, sizeof(float));
    out[i] = value;
  }

  *outCount = lengthCommon;
  return out;
}

GLuint* getIndices(const Model* self, const json* accessor, u32* outCount) {
  u32 buffViewInd = 0;
  u32 accByteOffset = 0;
  u32 byteOffset = 0; // Also not required by GLTF, so it should have 0 as the default value

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

  u32 outItemsCount;
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
        out[i] = (GLuint)value;
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
        out[i] = (GLuint)value;
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
        out[i] = (GLuint)value;
      }
      break;
    default:
      printf("Unhandled component type\n");
      exit(EXIT_FAILURE);
      break;
  }

  *outCount = outItemsCount;
  return out;
}

void getTextures(Model* self) {
  static Texture cachedTextures[MODEL_CACHED_TEXTURES_LENGTH];
  static u32 cachedTexturesIdx = 0;

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
    u32 pathLength = strlen(self->dirPath) + strlen(uriStr) + 1;
    char path[pathLength];
    concat(self->dirPath, uriStr, path, sizeof(char) * pathLength);

    bool skip = false;
    for (u32 j = 0; j < cachedTexturesIdx; j++) {
      if (strcmp(cachedTextures[j].name, uriStr)) {
        assert(self->texturesIdx < MODEL_TEXTURES_LENGTH);
        self->textures[self->texturesIdx++] = &cachedTextures[j];
        skip = true;
        break;
      }
    }

    if (!skip) {
      char* texType;

      if (strstr(uriStr, "baseColor"))
        texType = "diffuse";
      else if (strstr(uriStr, "metallicRoughness"))
        texType = "specular";
      else {
        printf("Unhandled texture type\n");
        exit(EXIT_FAILURE);
      }

      assert(cachedTexturesIdx < MODEL_CACHED_TEXTURES_LENGTH);
      cachedTextures[cachedTexturesIdx++] = textureCreate(path, texType);

      assert(self->texturesIdx < MODEL_TEXTURES_LENGTH);
      self->textures[self->texturesIdx++] = &cachedTextures[cachedTexturesIdx - 1];
    }
  }
}

float* assembleVertices(float* positions, float* normals, float* texUVs, u32 count) {
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

  getTextures(self);

  Object mesh = objectCreate(vertices, sizeof(float) * posVecsCount * OBJECT_VERTEX_ATTRIBUTES, indices, indicesCount * sizeof(GLuint), self->shader);
  for (int i = 0; i < self->texturesIdx; i++)
    objectAddTexture(&mesh, self->textures[i]);

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
  versor rotation = {0.f, 0.f, 0.f, 1.f};
  json* rotationJson;
  if (json_object_object_get_ex(node, "rotation", &rotationJson)) {
    assert(json_object_array_length(rotationJson) == 4);
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
  mat4 rot = GLM_MAT4_IDENTITY_INIT;
  mat4 sca = GLM_MAT4_IDENTITY_INIT;

  glm_translate(trans, translation);
  glm_quat_mat4(rotation, rot);
  glm_scale(sca, scale);

  mat4 matNextNode = GLM_MAT4_IDENTITY_INIT;
  glm_mat4_mul(matrix, matNode, matNextNode);
  glm_mat4_mul(matNextNode, trans, matNextNode);
  glm_mat4_mul(matNextNode, rot, matNextNode);
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
  u32 gltfPathLength = strlen(modelDirectory) + strlen(sceneGLTF) + 1;
  char gltfPath[gltfPathLength];
  concat(modelDirectory, sceneGLTF, gltfPath, sizeof(char) * gltfPathLength);

  // Read .gltf
  char* buffer = readFile(gltfPath, false);

  Model model;
  model.dirPath = modelDirectory;
  model.json = json_tokener_parse(buffer);
  model.data = getDataBin(&model);
  model.shader = shader;
  model.texturesIdx = 0;
  model.meshesIdx = 0;
  model.meshesSize = sizeof(Object);
  model.meshes = malloc(model.meshesSize);
  model.tmIdx = 0;
  model.tmSize = sizeof(vec3s);
  model.translationMeshes = malloc(model.tmSize);
  model.rmIdx = 0;
  model.rmSize = sizeof(vec4s);
  model.rotationMeshes = malloc(model.rmSize);
  model.smIdx = 0;
  model.smSize = sizeof(vec3s);
  model.scaleMeshes = malloc(model.smSize);
  model.mmIdx = 0;
  model.mmSize = sizeof(mat4s);
  model.matMeshes = malloc(model.mmSize);

  mat4 traverseMatInit = GLM_MAT4_IDENTITY_INIT;
  traverseNode(&model, 0, traverseMatInit);

  free(buffer);

  return model;
}

void modelDraw(const Model* self, const Camera* camera) {
  vec3s translation = {0.f, 0.f, 0.f};
  versors rotation = {0.f, 0.f, 0.f, 1.f};
  vec3s scale = {1.f, 1.f, 1.f};

  assert(self->meshesIdx == self->mmIdx);
  for (int i = 0; i < self->meshesIdx; i++)
    objectDraw(&self->meshes[i], camera, self->matMeshes[i], translation, rotation, scale);
}

void modelDelete(Model* self) {
  free(self->data);
  json_object_put(self->json);

  for (int i = 0; i < self->meshesIdx; i++)
    objectDelete(&self->meshes[i]);
  free(self->meshes);

  free(self->translationMeshes);
  free(self->rotationMeshes);
  free(self->scaleMeshes);
  free(self->matMeshes);
}

