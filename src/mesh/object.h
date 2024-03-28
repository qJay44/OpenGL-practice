#ifndef OBJECT_H
#define OBJECT_H

#include "cglm/types-struct.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"

typedef struct {
  const float* vertPtr;
  int vertSize;
  int vertCount;
  const GLuint* indPtr;
  int indSize;
  int indCount;

  GLint shaderProgram;
  struct VAO vao;
  struct VBO vbo;
  struct EBO ebo;
  mat4s mat;
} Object;

[[nodiscard]]
Object objectCreate(const float* vertices, int vertSize, const GLuint* indices, int indSize);

void objectLoadShaders(Object* self, const char* vsPath, const char* fsPath);
void objectBind(Object* self);
void objectUnbind();
void objectLinkAttrib(Object* self, GLuint loc, GLuint numComponents, GLuint strideLen, unsigned long long offsetLen);
void objectTranslate(Object* self, vec3s v);
void objectSetMatrixUniform(Object* self, const char* name);
void objectSetVec3Unifrom(Object* self, const char* name, vec3s v);
void objectSetVec4Unifrom(Object* self, const char* name, vec4s v);
void objectDraw(Object* self);
void objectDelete(Object* self);

#endif

