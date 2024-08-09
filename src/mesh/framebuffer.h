#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "fbo.h"
#include "rbo.h"
#include "texture.h"

typedef struct {
  struct VAO vaoRect;
  struct VBO vboRect;
  struct FBO fbo;
  struct RBO rbo;

  Texture texture;
} Framebuffer;

[[nodiscard]]
Framebuffer framebufferCreate(GLenum targetType);

void framebufferBind(const Framebuffer* self);
void framebufferUnbind(void);
void framebufferDraw(const Framebuffer* self, GLint shader, double time);

#endif

