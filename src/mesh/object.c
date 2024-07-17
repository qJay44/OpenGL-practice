#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cglm/struct/affine-pre.h"
#include "cglm/struct/mat4.h"
#include "cglm/quat.h"

#include "texture.h"
#include "object.h"

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

  // ===== Link attributes ===== //

  size_t typeSize = sizeof(GLfloat);
  size_t stride = OBJECT_VERTEX_ATTRIBUTES * typeSize;

  vaoLinkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vaoLinkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));
  vaoLinkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize));
  vaoLinkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize));

  // =========================== //

  vboUnbind();
  vaoUnbind();
  eboUnbind();

  return obj;
}

Object objectCreatePyramid(const GLint* shader) {
  float vertices[40] = {
    -0.5f, 0.0f,  0.5f,  0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,
    -0.5f, 0.0f, -0.5f,  0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,
     0.5f, 0.0f, -0.5f,  0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,
     0.5f, 0.0f,  0.5f,  0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,
     0.0f, 0.8f,  0.0f,  0.92f, 0.86f, 0.76f,	 2.5f, 5.0f
  };

  GLuint indices[18] = {
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
  };

  return objectCreate(vertices, sizeof(float) * 40, indices, sizeof(GLuint) * 18, shader);
}


void objectAddTexture(Object* self, Texture* tex) {
  if (self->texsCount < OBJECT_MAX_TEXTURES)
    self->textures[self->texsCount++] = tex;
  else
    printf("Warning: trying to add a texture to the object when reached maximum amount (%d)\n", OBJECT_MAX_TEXTURES);
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
  glUseProgram(*self->shaderProgram);
  GLuint uniTex = glGetUniformLocation(*self->shaderProgram, name);
  glUniform1i(uniTex, slot);
}

void objectSetCameraMatrixUnifrom(const Object* self, const GLfloat* mat, const char* name) {
  glUseProgram(*self->shaderProgram);
  GLint loc = glGetUniformLocation(*self->shaderProgram, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
}

void objectDraw(const Object* self, const Camera* camera, mat4s matrix, vec3s translation, versors rotation, vec3s scale) {
  glUseProgram(*self->shaderProgram);
  glBindVertexArray(self->vao.id);

  u8 numDiffuse = 0;
  u8 numSpecular = 0;

  for (int i = 0; i < self->texsCount; i++) {
    const char* texType = self->textures[i]->type;
    char numStr[3];
    u8 uniformStrLength;
    assert(numDiffuse < 256);

    // BUG: the shader has only diffuse0/specular0
    if (strcmp(texType, "diffuse")) {
      uniformStrLength = 7 + 3;
      sprintf(numStr, "%d", numDiffuse++);

    } else if (strcmp(texType, "specular")) {
      uniformStrLength = 8 + 3;
      sprintf(numStr, "%d", numSpecular++);
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
  glm_quat_mat4(rotation.raw, rot.raw);
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

  for (int i = 0; i < self->texsCount; i++)
    textureDelete(self->textures[i], 1);

  glDeleteProgram(*self->shaderProgram);
}

