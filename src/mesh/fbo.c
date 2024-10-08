#include "fbo.h"

struct FBO fboCreate(GLsizei size) {
  struct FBO fbo;
  glGenFramebuffers(size, &fbo.id);
  fbo.size = size;

  return fbo;
}

void fboBind(GLenum target, const struct FBO* self) {
  glBindFramebuffer(target, self->id);
}

void fboUnbind(void) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void fboDelete(const struct FBO* self) {
  glDeleteFramebuffers(self->size, &self->id);
}

