#ifndef OBJECT_H
#define OBJECT_H

#include "../camera.h"
#include "cglm/types-struct.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "texture.h"

#define OBJECT_MAX_TEXTURES 8

typedef struct {
  float* vertices;
  size_t vertSize;
  GLuint* indices;
  size_t indSize;

  const GLint* shaderProgram;
  struct VAO vao;
  struct VBO vbo;
  struct EBO ebo;
  mat4s mat;

  Texture* textures[OBJECT_MAX_TEXTURES];
  u32 texsCount;
} Object;

[[nodiscard]]
Object objectCreate(float* vertices, size_t vertSize, GLuint* indices, size_t indSize, const GLint* shader);

void objectAddTexture(Object* self, Texture* tex);
void objectTranslate(Object* self, vec3s v);
void objectSetMatrixUniform(const Object* self, const char* name);
void objectSetVec3Unifrom(const Object* self, const char* name, vec3s v);
void objectSetVec4Unifrom(const Object* self, const char* name, vec4s v);
void objectSetTextureUnifrom(const Object* self, const char* name, GLuint slot);
void objectSetCameraMatrixUnifrom(const Object* self, const GLfloat* mat, const char* name);
void objectDraw(const Object* self, const Camera* camera, mat4s matrix, vec3s translation, vec4s rotation, vec3s scale);
void objectDelete(Object* self);

#endif

