#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cglm/struct/affine-pre.h"
#include "cglm/struct/cam.h"
#include "cglm/struct/mat4.h"

#include "vao.h"
#include "ebo.h"
#include "texture.h"
#include "vbo.h"
#include "object.h"

// My order: positions (3), colors (3), texture coords (2), normals (3)
// Tutorial order: positions (3), normals (3), colors (3), texture coords (2)

#define CACHED_TEXTURES_LENGTH 20

Object objectCreate(float* vertices, size_t vertSize, GLuint* indices, size_t indSize) {
  Object obj;
  obj.vertices = malloc(vertSize);
  obj.vertSize = vertSize;
  obj.indices = malloc(indSize);
  obj.indSize = indSize;
  obj.mat = (mat4s)GLMS_MAT4_IDENTITY_INIT;
  obj.texturesIdx = 0;
  obj.instacing = 1;

  memcpy((void*)obj.vertices, (void*)vertices, vertSize);
  memcpy((void*)obj.indices, (void*)indices, indSize);

  obj.vao = vaoCreate(1);
  obj.vbo = vboCreate(1, obj.vertices, obj.vertSize);
  obj.ebo = eboCreate(1, obj.indices, obj.indSize);

  // Bind
  vaoBind(&obj.vao);
  vboBind(&obj.vbo);
  eboBind(&obj.ebo);

  // ===== Link attributes ===== //

  size_t typeSize = sizeof(GLfloat);
  size_t stride = OBJECT_VERTEX_ATTRIBUTES * typeSize;

  vaoLinkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize)); // 1. Position
  vaoLinkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize)); // 2. Color
  vaoLinkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize)); // 3. Texture
  vaoLinkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize)); // 4. Normal

  // =========================== //

  // Unbind
  vaoUnbind();
  vboUnbind();
  eboUnbind();

  return obj;
}

Object objectCreateInstancing(float* vertices, size_t vertSize, GLuint* indices, size_t indSize, mat4s* mat, size_t matSize, u32 instanceCount) {
  Object obj = objectCreate(vertices, vertSize, indices, indSize);
  obj.instacing = instanceCount;
  struct VBO vboInstance = vboCreate(1, mat, matSize);

  vaoBind(&obj.vao);
  vboBind(&obj.vbo);
  eboBind(&obj.ebo);
  vboBind(&vboInstance);

  // Can't link to a mat4 so linking four vec4's
  vaoLinkAttrib(4, 4, GL_FLOAT, sizeof(mat4s), (void*)0);
  vaoLinkAttrib(5, 4, GL_FLOAT, sizeof(mat4s), (void*)(1 * sizeof(vec4)));
  vaoLinkAttrib(6, 4, GL_FLOAT, sizeof(mat4s), (void*)(2 * sizeof(vec4)));
  vaoLinkAttrib(7, 4, GL_FLOAT, sizeof(mat4s), (void*)(3 * sizeof(vec4)));

  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);
  glVertexAttribDivisor(7, 1);

  vaoUnbind();
  vboUnbind();
  eboUnbind();

  return obj;
}

Object objectCreateSkybox(const char* texDirPath) {
  float vertices[24] = {
    //   Coordinates
    -1.0f, -1.0f,  1.0f,//        7--------6
     1.0f, -1.0f,  1.0f,//       /|       /|
     1.0f, -1.0f, -1.0f,//      4--------5 |
    -1.0f, -1.0f, -1.0f,//      | |      | |
    -1.0f,  1.0f,  1.0f,//      | 3------|-2
     1.0f,  1.0f,  1.0f,//      |/       |/
     1.0f,  1.0f, -1.0f,//      0--------1
    -1.0f,  1.0f, -1.0f
  };

  GLuint indices[36] = {
    // Right
    1, 2, 6,
    6, 5, 1,
    // Left
    0, 4, 7,
    7, 3, 0,
    // Top
    4, 5, 6,
    6, 7, 4,
    // Bottom
    0, 3, 2,
    2, 1, 0,
    // Back
    0, 1, 5,
    5, 4, 0,
    // Front
    3, 7, 6,
    6, 2, 3
  };

  Object obj;
  obj.vertices = vertices,
  obj.vertSize = sizeof(vertices),
  obj.indices = indices,
  obj.indSize = sizeof(indices),
  obj.mat = (mat4s)GLMS_MAT4_IDENTITY_INIT,
  obj.vao = vaoCreate(1),
  obj.vbo = vboCreate(1, obj.vertices, obj.vertSize),
  obj.ebo = eboCreate(1, obj.indices, obj.indSize),
  obj.texturesIdx = 0,
  obj.instacing = 1;

  // Bind
  vaoBind(&obj.vao);
  vboBind(&obj.vbo);
  eboBind(&obj.ebo);

  // Link attributes
  vaoLinkAttrib(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

  // Unbind
  vboUnbind();
  vaoUnbind();
  eboUnbind();

  obj.textures[obj.texturesIdx++] = textureCreateCubemap(texDirPath);

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

Object objectCreateTestLight(vec3s color) {
  // Positions (3), colors (3), texture coords (2), normals (3)
  GLfloat vertices[88] = {
	  -0.1f, -0.1f,  0.1f,  color.x, color.y, color.z,  0.f, 0.f,  0.f, 0.f, 0.f,
	  -0.1f, -0.1f, -0.1f,  color.x, color.y, color.z,  0.f, 0.f,  0.f, 0.f, 0.f,
	   0.1f, -0.1f, -0.1f,  color.x, color.y, color.z,  0.f, 0.f,  0.f, 0.f, 0.f,
	   0.1f, -0.1f,  0.1f,  color.x, color.y, color.z,  0.f, 0.f,  0.f, 0.f, 0.f,
	  -0.1f,  0.1f,  0.1f,  color.x, color.y, color.z,  0.f, 0.f,  0.f, 0.f, 0.f,
	  -0.1f,  0.1f, -0.1f,  color.x, color.y, color.z,  0.f, 0.f,  0.f, 0.f, 0.f,
	   0.1f,  0.1f, -0.1f,  color.x, color.y, color.z,  0.f, 0.f,  0.f, 0.f, 0.f,
	   0.1f,  0.1f,  0.1f,  color.x, color.y, color.z,  0.f, 0.f,  0.f, 0.f, 0.f,
  };

  GLuint indices[36] = {
	  0, 1, 2,
	  0, 2, 3,
	  0, 4, 7,
	  0, 7, 3,
	  3, 7, 6,
	  3, 6, 2,
	  2, 6, 5,
	  2, 5, 1,
	  1, 5, 4,
	  1, 4, 0,
	  4, 5, 6,
	  4, 6, 7
  };

  return objectCreate(vertices, sizeof(float) * 88, indices, sizeof(GLuint) * 36);
}

Object objectCreateTestPlane(void) {
  float vertices[44] = {
    -1.f, -1.f, 0.f,   0.f, 0.f, 0.f,   0.f, 0.f,   0.f, 0.f, 1.f,
    -1.f,  1.f, 0.f,   0.f, 0.f, 0.f,   0.f, 1.f,   0.f, 0.f, 1.f,
     1.f,  1.f, 0.f,   0.f, 0.f, 0.f,   1.f, 1.f,   0.f, 0.f, 1.f,
     1.f, -1.f, 0.f,   0.f, 0.f, 0.f,   1.f, 0.f,   0.f, 0.f, 1.f,
  };

  GLuint indices[6] = {
    0, 1, 2,
    0, 2, 3
  };

  return objectCreate(vertices, sizeof(float) * 44, indices, sizeof(GLuint) * 6);
}

void objectAddTexture(Object* self, const char* name, const char* path) {
  static Texture cachedTextures[CACHED_TEXTURES_LENGTH];
  static u32 cachedTexturesIdx = 0;

  bool skip = false;
  for (u32 j = 0; j < cachedTexturesIdx; j++) {
    if (!strcmp(cachedTextures[j].name, name)) {
      self->textures[self->texturesIdx++] = &cachedTextures[j];
      skip = true;
      break;
    }
  }

  if (!skip) {
    enum TextureEnum texType;

    if (strstr(name, "baseColor") || strstr(name, "diffuse"))
      texType = TEXTURE_DIFFUSE;
    else if (strstr(name, "metallicRoughness") || strstr(name, "specular"))
      texType = TEXTURE_SPECULAR;
    else if (strstr(name, "normal"))
      texType = TEXTURE_NORMAL;
    else if (strstr(name, "displacement"))
      texType = TEXTURE_DISPLACEMENT;
    else {
      printf("Unhandled texture type (%s)\n", name);
      return;
    }

    assert(cachedTexturesIdx < CACHED_TEXTURES_LENGTH);
    cachedTextures[cachedTexturesIdx++] = textureCreate2D(path, texType);

    if (self->texturesIdx < OBJECT_MAX_TEXTURES)
      self->textures[self->texturesIdx++] = &cachedTextures[cachedTexturesIdx - 1];
    else
      printf("Warning: trying to add a texture to the object when reached maximum amount (%d)\n", OBJECT_MAX_TEXTURES);
  }
}

void objectTranslate(Object* self, vec3s v) {
  self->mat = glms_translate(self->mat, v);
}

void objectDraw(const Object* self, const Camera* camera, GLint shader) {
  glUseProgram(shader);
  vaoBind(&self->vao);

  u8 numDiffuse = 0;
  u8 numSpecular = 0;
  u8 numNormal = 0;
  u8 numDisplacement = 0;

  for (int i = 0; i < self->texturesIdx; i++) {
    char uniform[256];
    switch (self->textures[i]->type) {
      case TEXTURE_DIFFUSE:
        sprintf(uniform, "diffuse%d", numDiffuse++);
        break;
      case TEXTURE_SPECULAR:
        sprintf(uniform, "specular%d", numSpecular++);
        break;
      case TEXTURE_NORMAL:
        sprintf(uniform, "normal%d", numNormal++);
        break;
      case TEXTURE_DISPLACEMENT:
        sprintf(uniform, "displacement%d", numDisplacement++);
        break;
      default:
        continue;
    }

    textureSetUniform(shader, uniform, self->textures[i]->unit);
    textureBind(self->textures[i]);
  }

  glUniform3f(glGetUniformLocation(shader, "camPosUni"), camera->position.x, camera->position.y, camera->position.y);
  glUniformMatrix4fv(glGetUniformLocation(shader, "cam"), 1, GL_FALSE, (const GLfloat*)camera->mat.raw);

  if (self->instacing == 1) {
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (const GLfloat*)self->mat.raw);
    glDrawElements(GL_TRIANGLES, self->indSize / sizeof(self->indices[0]), GL_UNSIGNED_INT, 0);
  } else {
    glDrawElementsInstanced(GL_TRIANGLES, self->indSize / sizeof(self->indices[0]), GL_UNSIGNED_INT, 0, self->instacing);
  }

  vaoUnbind();
}

void objectDrawSkybox(const Object* self, const Camera* camera, GLint shader) {
  glDepthFunc(GL_LEQUAL);
  glUseProgram(shader);
  mat4s view;
  mat4s proj;

  // Add empty last row and column;
  view = glms_mat4_ins3(
    // Cut the last row and column;
    glms_mat4_pick3(
      // mat4
      glms_lookat(camera->position, glms_vec3_add(camera->position, camera->orientation), camera->up)
    ),
    view
  );

  proj = glms_perspective(glm_rad(camera->fov), (float)_gState.winWidth / _gState.winHeight, _gState.nearPlane, _gState.farPlane);

  glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, (const GLfloat*)view.raw);
  glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, (const GLfloat*)proj.raw);

  vaoBind(&self->vao);
  textureBind(self->textures[0]);

  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  vaoUnbind();
  glDepthFunc(GL_LESS);
}

void objectDelete(Object* self) {
  vaoDelete(&self->vao);
  vboDelete(&self->vbo);
  eboDelete(&self->ebo);

  free(self->vertices);
  free(self->indices);

  for (int i = 0; i < self->texturesIdx; i++)
    textureDelete(self->textures[i], 1);
}

