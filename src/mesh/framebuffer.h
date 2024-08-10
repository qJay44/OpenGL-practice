#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "fbo.h"
#include "texture.h"

#define FRAMEBUFFER_T_DEFAULT     GL_TEXTURE_2D
#define FRAMEBUFFER_T_MULTISAMPLE GL_TEXTURE_2D_MULTISAMPLE

typedef struct {
  struct FBO fbo;
  Texture texture;
  int glType;
} Framebuffer;

[[nodiscard]]
Framebuffer framebufferCreate(GLenum fbType);

void framebufferBind(const Framebuffer* self);
void framebufferBindMultiSample(const Framebuffer* fbMultiSample, const Framebuffer* fbDefault);
void framebufferUnbind(void);
void framebufferDraw(const Framebuffer* self, GLint shader, const struct VAO* vao, u32 vertCount, double time);

#endif

