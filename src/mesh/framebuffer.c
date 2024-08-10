#include "framebuffer.h"

#include "fbo.h"
#include "rbo.h"
#include "texture.h"

Framebuffer framebufferCreate(GLenum targetType, bool createRBO) {
  Framebuffer fb;

  fb.fbo = fboCreate(1);
  fboBind(GL_FRAMEBUFFER, &fb.fbo);

  fb.texture = textureCreateFramebuffer(targetType);

  if (createRBO) {
    fb.rbo = rboCreate(1, targetType);
    rboUnbind();
  }

  fboUnbind();

  GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer error: %d\n", fboStatus);

  return fb;
}

void framebufferBind(const Framebuffer* self) {
  fboBind(GL_FRAMEBUFFER, &self->fbo);
}

void framebufferBindRead(const Framebuffer* self) {
  fboBind(GL_READ_FRAMEBUFFER, &self->fbo);

}
void framebufferBindDraw(const Framebuffer* self) {
  fboBind(GL_DRAW_FRAMEBUFFER, &self->fbo);
}

void framebufferUnbind(void) {
  fboUnbind();
}

void framebufferUse(const Framebuffer* self, GLint shader, double time) {
  textureBind(&self->texture);
}

