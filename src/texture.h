[[nodiscard]]
GLuint textureCreate(const char* path, GLenum texType, GLenum slot, GLenum formatIn, GLenum formatOut) {
  int imgWidth, imgHeight, imgColorChannels;
  stbi_set_flip_vertically_on_load(TRUE);
  unsigned char* imgBytes = stbi_load(path, &imgWidth, &imgHeight, &imgColorChannels, 0);

  GLuint texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(texType, texture);

  glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(texType, 0, formatOut, imgWidth, imgHeight, 0, formatIn, GL_UNSIGNED_BYTE, imgBytes);
  glGenerateMipmap(texType);

  stbi_image_free(imgBytes);
  glBindTexture(texType, 0); // Unbind

  return texture;
}

void textureBind(GLuint texture, GLenum texType) {
  glBindTexture(texType, texture);
}

void textureUnbind(GLenum texType) {
  glBindTexture(texType, 0);
}

void textureSetUniform(GLint shaderProgram, const char* name, GLuint unit) {
  GLuint uniTex = glGetUniformLocation(shaderProgram, name);
  glUseProgram(shaderProgram);
  glUniform1i(uniTex, unit);
}

void textureDelete(GLuint* texture, GLsizei num) {
  glDeleteTextures(num, texture);
}

