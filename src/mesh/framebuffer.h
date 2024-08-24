#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "fbo.h"

enum ShadowMapEnum {
  SHADOWMAP_DEFAULT = GL_TEXTURE_2D,
  SHADOWMAP_CUBEMAP = GL_TEXTURE_CUBE_MAP,
};

typedef struct {
  struct FBO fbo;
  Texture texture;
  int glType;
} Framebuffer;

[[nodiscard]] Framebuffer framebufferCreate(void);
[[nodiscard]] Framebuffer framebufferCreateMSAA(void);
[[nodiscard]] Framebuffer framebufferCreateShadowMap(enum ShadowMapEnum target, int w, int h);

void framebufferBind(const Framebuffer* self);
void framebufferBindReadDraw(const Framebuffer* read, const Framebuffer* draw);
void framebufferUnbind(void);
void framebufferDraw(GLint shader, const struct VAO* vao, u32 vertCount);

#endif

