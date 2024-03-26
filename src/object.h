#ifndef OBJECT_H
#define OBJECT_H

#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "cglm/struct.h"

typedef struct {
  float* vertPtr;
  int vertSize;
  int vertCount;
  GLuint* indPtr;
  int indSize;
  int indCount;

  GLint shaderProgram;
  struct VAO vao;
  struct VBO vbo;
  struct EBO ebo;
  mat4s mat;
} Object;

void objectLoadShaders(Object* self, const char* vsPath, const char* fsPath) {
  // A vertex shader reference
  GLint vertexShader = shaderLoad(vsPath, GL_VERTEX_SHADER);
  shaderCompile(vertexShader);

  // A fragment shader reference
  GLint fragmentShader = shaderLoad(fsPath, GL_FRAGMENT_SHADER);
  shaderCompile(fragmentShader);

  GLint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  shaderProgramLink(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  self->shaderProgram = shaderProgram;
}

void objectBind(Object* self) {
  struct VAO vao = vaoCreate(1);
  struct VBO vbo = vboCreate(1);
  struct EBO ebo = eboCreate(1);

  vaoBind(&vao);
  vboBind(&vbo, self->vertPtr, self->vertSize);
  eboBind(&ebo, self->indPtr, self->indSize);

  self->vao = vao;
  self->vbo = vbo;
  self->ebo = ebo;
}

void objectUnbind() {
  vboUnbind();
  vaoUnbind();
  eboUnbind();
}

void objectLinkAttrib(Object* self, GLuint loc, GLuint numComponents, GLuint strideLen, unsigned long long offsetLen) {
  unsigned int typeSize = sizeof(GLfloat);
  vaoLinkAttrib(loc, numComponents, GL_FLOAT, strideLen * typeSize, (void*)(offsetLen * typeSize));
}

void objectTranslate(Object* self, vec3s v) {
  self->mat = glms_translate(self->mat, v);
}

void objectSetMatrixUniform(Object* self, const char* name) {
  glUseProgram(self->shaderProgram);
  GLint loc = glGetUniformLocation(self->shaderProgram, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)&self->mat.raw);
}

void objectSetVec3Unifrom(Object* self, const char* name, vec3s v) {
  glUseProgram(self->shaderProgram);
  GLint loc = glGetUniformLocation(self->shaderProgram, name);
  glUniform3f(loc, v.x, v.y, v.z);
}

void objectSetVec4Unifrom(Object* self, const char* name, vec4s v) {
  glUseProgram(self->shaderProgram);
  GLint loc = glGetUniformLocation(self->shaderProgram, name);
  glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void objectDraw(Object* self) {
  glBindVertexArray(self->vao.id);
  glDrawElements(GL_TRIANGLES, self->indCount, GL_UNSIGNED_INT, 0);
}

void objectDelete(Object* self) {
  vaoDelete(&self->vao);
  vboDelete(&self->vbo);
  eboDelete(&self->ebo);
  glDeleteProgram(self->shaderProgram);
}

#endif

