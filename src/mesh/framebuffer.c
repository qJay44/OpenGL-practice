#include "framebuffer.h"

#include "fbo.h"
#include "rbo.h"
#include "texture.h"
#include "vao.h"
#include "vbo.h"

static const float rectangleVertices[] = {
  // Coords    // texCoords
   1.0f, -1.0f,  1.0f, 0.0f,
  -1.0f, -1.0f,  0.0f, 0.0f,
  -1.0f,  1.0f,  0.0f, 1.0f,

   1.0f,  1.0f,  1.0f, 1.0f,
   1.0f, -1.0f,  1.0f, 0.0f,
  -1.0f,  1.0f,  0.0f, 1.0f
};

Framebuffer framebufferCreate(GLenum targetType) {
  Framebuffer fb;

  fb.vaoRect = vaoCreate(1);
  fb.vboRect = vboCreate(1, rectangleVertices, sizeof(rectangleVertices));

  vaoBind(&fb.vaoRect);
  vboBind(&fb.vboRect);

  vaoLinkAttrib(0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
  vaoLinkAttrib(1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  fb.fbo = fboCreate(1);
  fboBind(&fb.fbo);

  fb.texture = textureCreateFramebuffer(targetType);

  fb.rbo = rboCreate(1);

  vaoUnbind();
  vboUnbind();
  rboUnbind();

  return fb;
}

void framebufferBind(const Framebuffer* self) {
  glBindFramebuffer(GL_FRAMEBUFFER, self->fbo.id);
}

void framebufferUnbind(void) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebufferDraw(const Framebuffer* self, GLint shader, double time) {
  glUseProgram(shader);
  vaoBind(&self->vaoRect);
  glDisable(GL_DEPTH_TEST);
  textureBind(&self->texture);

	glUniform1i(glGetUniformLocation(shader, "winWidth"), _gState.winWidth);
	glUniform1i(glGetUniformLocation(shader, "winHeight"), _gState.winHeight);
	glUniform1f(glGetUniformLocation(shader, "time"), time);

  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_DEPTH_TEST);
}

