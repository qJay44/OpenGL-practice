#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "fbo.h"
#include "rbo.h"
#include "texture.h"

typedef struct {
  struct FBO fbo;
  struct RBO rbo;

  Texture texture;
} Framebuffer;

[[nodiscard]]
Framebuffer framebufferCreate(GLenum targetType, bool createRBO);

void framebufferBind(const Framebuffer* self);
void framebufferBindRead(const Framebuffer* self);
void framebufferBindDraw(const Framebuffer* self);
void framebufferUnbind(void);
void framebufferUse(const Framebuffer* self, GLint shader, double time);

#endif

