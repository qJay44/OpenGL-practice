#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  GLuint id;
  GLuint slot;
  char type[256];
  char name[256];
} Texture;

[[nodiscard]]
Texture textureCreate(const char* path, const char* type);
void textureBind(const Texture* self);
void textureUnbind(GLenum texType);
void textureUnit(GLint shader, const char* uniform, GLuint unit);
void textureDelete(Texture* self, GLsizei num);

#endif

