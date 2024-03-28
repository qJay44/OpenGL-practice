#include "object.h"
#include "cglm/struct/affine-pre.h"
#include "cglm/struct/mat4.h"
#include "shader.h"

Object objectCreate(const float* vertices, int vertSize, const GLuint* indices, int indSize) {
  static const unsigned char floatSize = sizeof(float);
  static const unsigned char GLuintSize = sizeof(GLuint);

  Object obj = {
    .vertPtr = vertices,
    .vertSize = vertSize,
    .vertCount = vertSize / floatSize,
    .indPtr = indices,
    .indSize = indSize,
    .indCount = indSize / GLuintSize,
    .mat = GLMS_MAT4_IDENTITY_INIT,
    .vao = vaoCreate(1),
    .vbo = vboCreate(1),
    .ebo = eboCreate(1)
  };

  return obj;
}

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
  vaoBind(&self->vao);
  vboBind(&self->vbo, self->vertPtr, self->vertSize);
  eboBind(&self->ebo, self->indPtr, self->indSize);
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

