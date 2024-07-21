#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cglm/struct/affine-pre.h"
#include "cglm/struct/mat4.h"
#include "cglm/quat.h"

#include "texture.h"
#include "vao.h"
#include "object.h"

Object objectCreate(float* vertices, size_t vertSize, GLuint* indices, size_t indSize) {
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

  vaoLinkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize)); // 1. Position
  vaoLinkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize)); // 2. Color
  vaoLinkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize)); // 3. Texture
  vaoLinkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize)); // 4. Normal

  // =========================== //

  // Unbind
  vboUnbind();
  vaoUnbind();
  eboUnbind();

  return obj;
}

Object objectCreateTestPyramid(void) {
  // Positions (3), colors (3), texture coords (2), normals (3)
  float vertices[176] = {
    -0.5f, 0.0f,  0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // Bottom side
    -0.5f, 0.0f, -0.5f,  0.83f, 0.70f, 0.44f,	 0.0f, 5.0f,  0.0f, -1.0f, 0.0f, // Bottom side
     0.5f, 0.0f, -0.5f,  0.83f, 0.70f, 0.44f,	 5.0f, 5.0f,  0.0f, -1.0f, 0.0f, // Bottom side
     0.5f, 0.0f,  0.5f,  0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,  0.0f, -1.0f, 0.0f, // Bottom side

    -0.5f, 0.0f,  0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 0.0f,  0.8f, 0.5f,  0.0f, // Left Side
    -0.5f, 0.0f, -0.5f,  0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,  0.8f, 0.5f,  0.0f, // Left Side
     0.0f, 0.8f,  0.0f,  0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,  0.8f, 0.5f,  0.0f, // Left Side

    -0.5f, 0.0f, -0.5f,  0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,  0.0f, 0.5f, -0.8f, // Non-facing side
     0.5f, 0.0f, -0.5f,  0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,  0.0f, 0.5f, -0.8f, // Non-facing side
     0.0f, 0.8f,  0.0f,  0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,  0.0f, 0.5f, -0.8f, // Non-facing side

     0.5f, 0.0f, -0.5f,  0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,  0.8f, 0.5f,  0.0f, // Right side
     0.5f, 0.0f,  0.5f,  0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,  0.8f, 0.5f,  0.0f, // Right side
     0.0f, 0.8f,  0.0f,  0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,  0.8f, 0.5f,  0.0f, // Right side

     0.5f, 0.0f,  0.5f,  0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,  0.0f, 0.5f,  0.8f, // Facing side
    -0.5f, 0.0f,  0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 0.0f,  0.0f, 0.5f,  0.8f, // Facing side
     0.0f, 0.8f,  0.0f,  0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,  0.0f, 0.5f,  0.8f  // Facing side
  };

  GLuint indices[18] = {
    0, 1, 2,    // Bottom side
    0, 2, 3,    // Bottom side
    4, 6, 5,    // Left side
    7, 9, 8,    // Non-facing side
    10, 12, 11, // Right side
    13, 15, 14  // Facing side
  };

  return objectCreate(vertices, sizeof(float) * 176, indices, sizeof(GLuint) * 18);
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

void objectSetMatrixUniform(const Object* self, const char* name, GLint shader) {
  glUseProgram(shader);
  GLint loc = glGetUniformLocation(shader, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)&self->mat.raw);
}

void objectSetVec3Unifrom(const Object* self, const char* name, GLint shader, vec3s v) {
  glUseProgram(shader);
  GLint loc = glGetUniformLocation(shader, name);
  glUniform3f(loc, v.x, v.y, v.z);
}

void objectSetVec4Unifrom(const Object* self, const char* name, GLint shader, vec4s v) {
  glUseProgram(shader);
  GLint loc = glGetUniformLocation(shader, name);
  glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void objectSetTextureUnifrom(const Object* self, const char* name, GLint shader, GLuint slot) {
  glUseProgram(shader);
  GLuint uniTex = glGetUniformLocation(shader, name);
  glUniform1i(uniTex, slot);
}

void objectSetCameraMatrixUnifrom(const Object* self, const GLfloat* mat, const char* name, GLint shader) {
  glUseProgram(shader);
  GLint loc = glGetUniformLocation(shader, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
}

void objectDraw(const Object* self, const Camera* camera, mat4s matrix, vec3s translation, versors rotation, vec3s scale, GLint shader) {
  glUseProgram(shader);
  vaoBind(&self->vao);

  u8 numDiffuse = 0;
  u8 numSpecular = 0;

  for (int i = 0; i < self->texsCount; i++) {
    const char* texType = self->textures[i]->type;
    char numStr[3];
    u8 uniformStrLength;
    assert(numDiffuse < 256);

    // REVIEW: The shader has only diffuse0/specular0
    if (!strcmp(texType, "diffuse")) {
      uniformStrLength = 7 + 3;
      sprintf(numStr, "%d", numDiffuse++);

    } else if (!strcmp(texType, "specular")) {
      uniformStrLength = 8 + 3;
      sprintf(numStr, "%d", numSpecular++);
    }

    char uniform[uniformStrLength + 1];
    concat(texType, numStr, uniform, uniformStrLength * sizeof(char));

    textureUnit(shader, uniform, i);
    textureBind(self->textures[i]);
  }

  objectSetVec3Unifrom(self, "camPos", shader, camera->position);
  objectSetCameraMatrixUnifrom(self, (const GLfloat*)camera->mat.raw, "camMat", shader);

  mat4s trans = GLMS_MAT4_IDENTITY_INIT;
  mat4s rot = GLMS_MAT4_IDENTITY_INIT;
  mat4s sca = GLMS_MAT4_IDENTITY_INIT;

  trans = glms_translate(trans, translation);
  glm_quat_mat4(rotation.raw, rot.raw);
  glm_scale(sca.raw, scale.raw);

  glUniformMatrix4fv(glGetUniformLocation(shader, "translation"), 1, GL_FALSE, (const GLfloat*)trans.raw);
  glUniformMatrix4fv(glGetUniformLocation(shader, "rotation"), 1, GL_FALSE, (const GLfloat*)rot.raw);
  glUniformMatrix4fv(glGetUniformLocation(shader, "scale"), 1, GL_FALSE, (const GLfloat*)sca.raw);
  glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (const GLfloat*)matrix.raw);

  glDrawElements(GL_TRIANGLES, self->indSize / sizeof(self->indices[0]), GL_UNSIGNED_INT, 0);
  vaoUnbind();
}

void objectDelete(Object* self) {
  vaoDelete(&self->vao);
  vboDelete(&self->vbo);
  eboDelete(&self->ebo);

  free(self->vertices);
  free(self->indices);

  for (int i = 0; i < self->texsCount; i++)
    textureDelete(self->textures[i], 1);
}

