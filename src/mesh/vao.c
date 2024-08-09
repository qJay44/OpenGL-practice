#include "vao.h"

struct VAO vaoCreate(GLsizei size) {
  struct VAO vao;
  glGenVertexArrays(size, &vao.id);
  vao.size = size;

  return vao;
}

void vaoBind(const struct VAO* self) {
  glBindVertexArray(self->id);
}

void vaoLinkAttrib(GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
  glEnableVertexAttribArray(layout);
  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
}

void vaoUnbind(void) {
  glBindVertexArray(0);
}

void vaoDelete(const struct VAO* self) {
  glDeleteVertexArrays(self->size, &self->id);
}

