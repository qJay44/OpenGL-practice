#include "texture.h"
#include <assert.h>
#include <gl/gl.h>

Texture textureCreate(const char* path, const char* type) {
  static u16 slot = 0;

  assert(slot < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);

  int imgWidth, imgHeight, imgColorChannels;
  stbi_set_flip_vertically_on_load(true);
  byte* imgBytes = stbi_load(path, &imgWidth, &imgHeight, &imgColorChannels, 0);

  GLuint textureId;
  glGenTextures(1, &textureId);
  glActiveTexture(slot);
  glBindTexture(GL_TEXTURE_2D, textureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  GLenum imgChannel;
  switch (imgColorChannels) {
    case 1: imgChannel = GL_RED; break;
    case 3: imgChannel = GL_RGB; break;
    case 4: imgChannel = GL_RGBA; break;
    default:
      printf("Unhandled texture image color channels type\n");
      exit(EXIT_FAILURE);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, imgChannel, GL_UNSIGNED_BYTE, imgBytes);

  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(imgBytes);
  glBindTexture(GL_TEXTURE_2D, 0); // Unbind

  Texture tex;
  tex.id = textureId;
  tex.slot = slot++;
  tex.type = type;
  tex.name = getFileNameFromPath(path);

  return tex;
}

void textureBind(const Texture* self) {
  glActiveTexture(self->slot);
  glBindTexture(GL_TEXTURE_2D, self->id);
}

void textureUnbind(GLenum texType) {
  glBindTexture(texType, 0);
}

void textureUnit(GLint shader, const char* uniform, GLuint unit) {
  GLuint texUni = glGetUniformLocation(shader, uniform);
  glUseProgram(shader);
  glUniform1i(texUni, unit);
}

void textureDelete(Texture* self, GLsizei num) {
  glDeleteTextures(num, &self->id);
}

