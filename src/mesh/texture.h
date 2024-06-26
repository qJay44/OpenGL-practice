#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  GLuint id;
  GLuint slot;
  const char* type;
} Texture;

[[nodiscard]]
Texture textureCreate(const char* path, const char* type, GLenum slot);
void textureBind(const Texture* self);
void textureUnbind(GLenum texType);
void textureDelete(Texture* self, GLsizei num);

#endif

