#include "texture.h"

Texture textureCreate(const char* path, const char* type, GLenum slot) {
  int imgWidth, imgHeight, imgColorChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* imgBytes = stbi_load(path, &imgWidth, &imgHeight, &imgColorChannels, 0);

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
      printf("Automatic texture type recognition failed\n");
      exit(EXIT_FAILURE);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, imgChannel, GL_UNSIGNED_BYTE, imgBytes);

  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(imgBytes);
  glBindTexture(GL_TEXTURE_2D, 0); // Unbind

  Texture tex = {
    .id = textureId,
    .slot = slot,
    .type = type
  };

  return tex;
}

void textureBind(const Texture* self) {
  glActiveTexture(self->slot);
  glBindTexture(GL_TEXTURE_2D, self->id);
}

void textureUnbind(GLenum texType) {
  glBindTexture(texType, 0);
}

void textureUnit(const GLint* shader, const char* uniform, GLuint unit) {
  GLuint texUni = glGetUniformLocation(*shader, uniform);
  glUseProgram(*shader);
  glUniform1i(texUni, unit);
}

void textureDelete(Texture* self, GLsizei num) {
  glDeleteTextures(num, &self->id);
}

