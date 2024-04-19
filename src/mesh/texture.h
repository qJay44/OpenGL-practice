#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  GLuint id;
  GLuint slot;
  GLenum type;
} Texture;

[[nodiscard]]
Texture textureCreate2D(const char* path, GLenum slot);
void textureBind(const Texture* self);
void textureUnbind(GLenum texType);
void textureDelete(Texture* self, GLsizei num);

#endif

