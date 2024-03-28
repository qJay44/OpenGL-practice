#ifndef VAO_H
#define VAO_H

// Vertex Array Object
struct VAO {
  GLuint id;
  GLsizei size;
};

[[nodiscard]]
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
  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  glEnableVertexAttribArray(layout);
}

void vaoUnbind(void) {
  glBindVertexArray(0);
}

void vaoDelete(const struct VAO* self) {
  glDeleteVertexArrays(self->size, &self->id);
}

#endif

