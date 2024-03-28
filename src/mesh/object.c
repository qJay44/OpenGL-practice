#include "object.h"
#include "cglm/struct/affine-pre.h"
#include "cglm/struct/mat4.h"
#include <stdio.h>

Object objectCreate(const float* vertices, int vertSize, const GLuint* indices, int indSize, const GLint* shader) {
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
    .ebo = eboCreate(1),
    .texturesAmount = 0,
  };

  obj.shaderProgram = shader;

  // Bind
  vaoBind(&obj.vao);
  vboBind(&obj.vbo, obj.vertPtr, obj.vertSize);
  eboBind(&obj.ebo, obj.indPtr, obj.indSize);

  // Link attributes //

  static const unsigned long long typeSize = sizeof(GLfloat);
  static const int stride = 11 * typeSize;

  vaoLinkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vaoLinkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));
  vaoLinkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize));
  vaoLinkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize));

  vboUnbind();
  vaoUnbind();
  eboUnbind();

  /////////////////////

  return obj;
}

void objectAddTexture(Object* self, const Texture* tex) {
  if (self->texturesAmount < 8)
    self->textures[self->texturesAmount++] = tex;
  else
    printf("Warning: trying to set a texture to the object when its reached maximum amount (8)\n");
}

void objectTranslate(Object* self, vec3s v) {
  self->mat = glms_translate(self->mat, v);
}

void objectSetMatrixUniform(const Object* self, const char* name) {
  glUseProgram(*self->shaderProgram);
  GLint loc = glGetUniformLocation(*self->shaderProgram, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)&self->mat.raw);
}

void objectSetVec3Unifrom(const Object* self, const char* name, vec3s v) {
  glUseProgram(*self->shaderProgram);
  GLint loc = glGetUniformLocation(*self->shaderProgram, name);
  glUniform3f(loc, v.x, v.y, v.z);
}

void objectSetVec4Unifrom(const Object* self, const char* name, vec4s v) {
  glUseProgram(*self->shaderProgram);
  GLint loc = glGetUniformLocation(*self->shaderProgram, name);
  glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void objectSetTextureUnifrom(const Object* self, const char* name, GLuint slot) {
  GLuint uniTex = glGetUniformLocation(*self->shaderProgram, name);
  glUseProgram(*self->shaderProgram);
  glUniform1i(uniTex, slot);
}

void objectSetCameraMatrixUnifrom(const Object* self, const GLfloat* mat, const char* name) {
  glUseProgram(*self->shaderProgram);
  GLint loc = glGetUniformLocation(*self->shaderProgram, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
}

void objectDraw(const Object* self) {
  for (int i = 0; i < self->texturesAmount; i++)
    textureBind(self->textures[i], GL_TEXTURE_2D);

  glBindVertexArray(self->vao.id);
  glDrawElements(GL_TRIANGLES, self->indCount, GL_UNSIGNED_INT, 0);
}

void objectDelete(Object* self) {
  vaoDelete(&self->vao);
  vboDelete(&self->vbo);
  eboDelete(&self->ebo);
  glDeleteProgram(*self->shaderProgram);
}

