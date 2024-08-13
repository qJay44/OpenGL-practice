#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "fbo.h"
#include "texture.h"

typedef struct {
  struct FBO fbo;
  Texture texture;
  int glType;
} Framebuffer;

[[nodiscard]] Framebuffer framebufferCreate(void);
[[nodiscard]] Framebuffer framebufferCreateMSAA(void);
[[nodiscard]] Framebuffer framebufferCreateShadowMap(int w, int h);

void framebufferBind(const Framebuffer* self);
void framebufferBindReadDraw(const Framebuffer* read, const Framebuffer* draw);
void framebufferUnbind(void);
void framebufferDraw(const Framebuffer* self, GLint shader, const struct VAO* vao, u32 vertCount);

#endif

