#ifndef OBJECT_H
#define OBJECT_H

#include "../camera.h"
#include "cglm/types-struct.h"

#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "texture.h"

#define OBJECT_MAX_TEXTURES 0xff
#define OBJECT_VERTEX_ATTRIBUTES 11 // 3 (position) + 3 (normal) + 3 (color) + 2 (texture)

typedef struct {
  float* vertices;
  size_t vertSize;
  GLuint* indices;
  size_t indSize;

  struct VAO vao;
  struct VBO vbo;
  struct EBO ebo;

  mat4s mat;
  u32 instacing;

  Texture* textures[OBJECT_MAX_TEXTURES];
  u32 texturesIdx;
} Object;

[[nodiscard]] Object objectCreate(float* vertices, size_t vertSize, GLuint* indices, size_t indSize);
[[nodiscard]] Object objectCreateInstancing(float* vertices, size_t vertSize, GLuint* indices, size_t indSize, mat4s* mat, size_t matSize, u32 instanceCount);
[[nodiscard]] Object objectCreateTestPyramid(void);
[[nodiscard]] Object objectCreateSkybox(const char* dirPath);
[[nodiscard]] Object objectCreateTestLight(vec3s color);

void objectAddTexture(Object* self, const char* name, const char* path);
void objectTranslate(Object* self, vec3s v);
void objectSetMatrixUniform(const Object* self, const char* name, GLint shader);
void objectSetVec3Unifrom(const Object* self, const char* name, GLint shader, vec3s v);
void objectSetVec4Unifrom(const Object* self, const char* name, GLint shader, vec4s v);
void objectSetTextureUnifrom(const Object* self, const char* name, GLint shader, GLuint slot);
void objectSetCameraMatrixUnifrom(const Object* self, const GLfloat* mat, const char* name, GLint shader);
void objectDraw(const Object* self, const Camera* camera, GLint shader);
void objectDrawSkybox(const Object* self, const Camera* camera, GLint shader);
void objectDelete(Object* self);

#endif

