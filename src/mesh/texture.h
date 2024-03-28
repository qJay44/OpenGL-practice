#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  GLuint id;
  GLuint slot;
} Texture;

[[nodiscard]]
Texture textureCreate(const char* path, GLenum texType, GLenum slot, GLenum formatIn, GLenum formatOut);
void textureBind(const Texture* self, GLenum texType);
void textureUnbind(GLenum texType);
void textureDelete(Texture* self, GLsizei num);

#endif

