#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  GLuint id;
  GLuint slot;
  const char* type;
  const char* name;
} Texture;

[[nodiscard]]
Texture textureCreate(const char* path, const char* type);
void textureBind(const Texture* self);
void textureUnbind(GLenum texType);
void textureUnit(GLint shader, const char* uniform, GLuint unit);
void textureDelete(Texture* self, GLsizei num);

#endif

