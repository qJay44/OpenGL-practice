#include "ebo.h"

struct EBO eboCreate(GLsizei size, const void* data, GLsizeiptr dataSize) {
  struct EBO ebo;
  glGenBuffers(size, &ebo.id);
  ebo.size = size;
  eboBind(&ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);

  return ebo;
}

void eboBind(const struct EBO* self) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->id);
}

void eboUnbind(void) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void eboDelete(const struct EBO* self) {
  glDeleteBuffers(self->size, &self->id);
}

