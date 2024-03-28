#include "ebo.h"

struct EBO eboCreate(GLsizei size) {
  struct EBO ebo;
  glGenBuffers(size, &ebo.id);
  ebo.size = size;

  return ebo;
}

void eboBind(const struct EBO* self, const void* data, GLsizeiptr dataSize) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
}

void eboUnbind(void) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void eboDelete(const struct EBO* self) {
  glDeleteBuffers(self->size, &self->id);
}

