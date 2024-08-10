#include "rbo.h"

struct RBO rboCreate(GLsizei size, GLenum targetType) {
  struct RBO rbo;
  glGenRenderbuffers(size, &rbo.id);
  rbo.size = size;

  rboBind(&rbo);

  if (targetType == GL_TEXTURE_2D_MULTISAMPLE)
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, _gState.aaSamples, GL_DEPTH24_STENCIL8,  _gState.winWidth, _gState.winHeight);
  else
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _gState.winWidth, _gState.winHeight);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo.id);

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

