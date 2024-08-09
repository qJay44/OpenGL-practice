#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  GLuint id;
  GLuint slot;
  char type[256]; // diffuse (also "baseColor"), specular (also "metallicRoughness")
  char name[256];
  GLenum glType; // GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP
} Texture;

[[nodiscard]] Texture textureCreate2D(const char* path, const char* type, u8 slot);
[[nodiscard]] Texture* textureCreateCubemap(const char* dirPath);
[[nodiscard]] Texture textureCreateFramebuffer(GLenum targetType);

void textureBind(const Texture* self);
void textureUnbind(GLenum texType);
void textureUnit(GLint shader, const char* uniform, GLuint unit);
void textureDelete(Texture* self, GLsizei num);

#endif

