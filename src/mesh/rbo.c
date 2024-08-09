#include "rbo.h"
#include <stdio.h>

struct RBO rboCreate(GLsizei size) {
  struct RBO rbo;
  glGenRenderbuffers(size, &rbo.id);
  rbo.size = size;

  rboBind(&rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _gState.winWidth, _gState.winHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo.id);

  GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer error: %d\n", fboStatus);

  return rbo;
}

void rboBind(const struct RBO* self) {
  glBindRenderbuffer(GL_RENDERBUFFER, self->id);
}

void rboUnbind(void) {
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void rboDelete(const struct RBO* self) {
  glDeleteRenderbuffers(self->size, &self->id);
}

