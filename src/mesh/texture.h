#ifndef TEXTURE_H
#define TEXTURE_H

enum TextureEnum : u8 {
  TEXTURE_DIFFUSE,
  TEXTURE_SPECULAR,
  TEXTURE_CUBEMAP,
  TEXTURE_FRAMEBUFFER,
};

typedef struct {
  GLuint id;
  GLuint unit;
  char name[256];
  enum TextureEnum type;
  GLenum glType; // GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP
} Texture;

[[nodiscard]] Texture textureCreate2D(const char* path, enum TextureEnum type);
[[nodiscard]] Texture* textureCreateCubemap(const char* dirPath);
[[nodiscard]] Texture textureCreateFramebuffer(GLenum targetType);

void textureBind(const Texture* self);
void textureUnbind(GLenum texType);
void textureSetUniform(GLint shader, const char* uniform, GLuint unit);
void textureDelete(Texture* self, GLsizei num);

#endif

