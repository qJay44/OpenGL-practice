#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  GLuint id;
  GLuint slot;
} Texture;

[[nodiscard]]
Texture textureCreate(const char* path, GLenum texType, GLenum slot, GLenum formatIn, GLenum formatOut) {
  int imgWidth, imgHeight, imgColorChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* imgBytes = stbi_load(path, &imgWidth, &imgHeight, &imgColorChannels, 0);

  GLuint textureId;
  glGenTextures(1, &textureId);
  glActiveTexture(slot);
  glBindTexture(texType, textureId);

  glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(texType, 0, formatOut, imgWidth, imgHeight, 0, formatIn, GL_UNSIGNED_BYTE, imgBytes);
  glGenerateMipmap(texType);

  stbi_image_free(imgBytes);
  glBindTexture(texType, 0); // Unbind

  Texture tex = {
    .id = textureId,
    .slot = slot
  };

  return tex;
}

void textureBind(Texture* self, GLenum texType) {
  glActiveTexture(self->slot);
  glBindTexture(texType, self->id);
}

void textureUnbind(GLenum texType) {
  glBindTexture(texType, 0);
}

void textureSetUniform(GLint shaderProgram, const char* name, GLuint unit) {
  GLuint uniTex = glGetUniformLocation(shaderProgram, name);
  glUseProgram(shaderProgram);
  glUniform1i(uniTex, unit);
}

void textureDelete(Texture* self, GLsizei num) {
  glDeleteTextures(num, &self->id);
}

#endif

