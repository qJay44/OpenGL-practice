#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "fbo.h"
#include "texture.h"

enum FramebufferEnum : GLenum {
  FRAMEBUFFER_DEFAULT     = GL_TEXTURE_2D,
  FRAMEBUFFER_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE
};

typedef struct {
  struct FBO fbo;
  Texture texture;
  int glType;
} Framebuffer;

[[nodiscard]]
Framebuffer framebufferCreate(enum FramebufferEnum fbType);

void framebufferBind(const Framebuffer* self);
void framebufferBindReadDraw(const Framebuffer* read, const Framebuffer* draw);
void framebufferUnbind(void);
void framebufferDraw(const Framebuffer* self, GLint shader, const struct VAO* vao, u32 vertCount);

#endif

