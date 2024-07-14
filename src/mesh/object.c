#include <stdio.h>
#include <string.h>

#include "object.h"
#include "cglm/struct/affine-pre.h"
#include "cglm/struct/mat4.h"
#include "texture.h"

Object objectCreate(float* vertices, size_t vertSize, GLuint* indices, size_t indSize, const GLint* shader) {
  Object obj = {
    .vertices = malloc(vertSize),
    .vertSize = vertSize,
    .indices = malloc(indSize),
    .indSize = indSize,
    .mat = GLMS_MAT4_IDENTITY_INIT,
    .vao = vaoCreate(1),
    .vbo = vboCreate(1),
    .ebo = eboCreate(1),
    .texsCount = 0,
    .shaderProgram = shader
  };
  memcpy((void*)obj.vertices, (void*)vertices, vertSize);
  memcpy((void*)obj.indices, (void*)indices, indSize);

  // Bind
  vaoBind(&obj.vao);
  vboBind(&obj.vbo, obj.vertices, obj.vertSize);
  eboBind(&obj.ebo, obj.indices, obj.indSize);

  // Link attributes //

  size_t typeSize = sizeof(GLfloat);
  size_t stride = 11 * typeSize;

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

void objectAddTexture(Object* self, Texture* tex) {
  if (self->texsCount < OBJECT_MAX_TEXTURES)
    self->textures[self->texsCount++] = tex;
  else
    printf("Warning: trying to add a texture to the object when its reached maximum amount (%d)\n", OBJECT_MAX_TEXTURES);
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

void objectDraw(const Object* self, const Camera* camera, mat4s matrix, vec3s translation, vec4s rotation, vec3s scale) {
  glUseProgram(*self->shaderProgram);
  glBindVertexArray(self->vao.id);

  u32 numDiffuse = 0;
  u32 numSpecular = 0;

  for (int i = 0; i < self->texsCount; i++) {
    const char* texType = self->textures[i]->type;
    char numStr[256];
    u8 uniformStrLength;

    if (strcmp(texType, "diffuse")) {
      uniformStrLength = numDiffuse + 7;
      int2str(numDiffuse++, numStr, 10);

    } else if (strcmp(texType, "specular")) {
      uniformStrLength = numSpecular + 8;
      int2str(numSpecular++, numStr, 10);
    }

    char uniform[uniformStrLength + 1];
    concat(texType, numStr, uniform, uniformStrLength * sizeof(char));

    textureUnit(self->shaderProgram, uniform, i);
    textureBind(self->textures[i]);
  }

  objectSetVec3Unifrom(self, "camPos", camera->position);
  objectSetCameraMatrixUnifrom(self, (const GLfloat*)camera->mat.raw, "matCam");

  mat4s trans = GLMS_MAT4_IDENTITY_INIT;
  mat4s rot = GLMS_MAT4_IDENTITY_INIT;
  mat4s sca = GLMS_MAT4_IDENTITY_INIT;

  trans = glms_translate(trans, translation);
  rot.m00 *= rotation.x;
  rot.m11 *= rotation.y;
  rot.m22 *= rotation.z;
  rot.m33 *= rotation.w;
  glm_scale(sca.raw, scale.raw);

  glUniformMatrix4fv(glGetUniformLocation(*self->shaderProgram, "translation"), 1, GL_FALSE, (const GLfloat*)trans.raw);
  glUniformMatrix4fv(glGetUniformLocation(*self->shaderProgram, "rotation"), 1, GL_FALSE, (const GLfloat*)rot.raw);
  glUniformMatrix4fv(glGetUniformLocation(*self->shaderProgram, "scale"), 1, GL_FALSE, (const GLfloat*)sca.raw);
  glUniformMatrix4fv(glGetUniformLocation(*self->shaderProgram, "model"), 1, GL_FALSE, (const GLfloat*)matrix.raw);

  glDrawElements(GL_TRIANGLES, self->indSize / sizeof(self->indices[0]), GL_UNSIGNED_INT, 0);
}

void objectDelete(Object* self) {
  vaoDelete(&self->vao);
  vboDelete(&self->vbo);
  eboDelete(&self->ebo);

  free(self->vertices);
  free(self->indices);

  for (int i = 0; i < self->texsCount; i++) {
    textureDelete(self->textures[i], 1);
    free(self->textures[i]);
  }

  glDeleteProgram(*self->shaderProgram);
}

