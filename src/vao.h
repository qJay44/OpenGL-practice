#ifndef VAO_H
#define VAO_H

// Vertex Array Object
struct VAO {
  GLuint id;
  GLsizei size;
};

struct VAO vaoCreate(GLsizei size) {
  struct VAO vao;
  glGenVertexArrays(size, &vao.id);
  vao.size = size;

  return vao;
}

void vaoBind(const struct VAO* vao) {
  glBindVertexArray(vao->id);
}

void vaoLinkAttrib(GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  glEnableVertexAttribArray(layout);
}

void vaoUnbind(void) {
  glBindVertexArray(0);
}

void vaoDelete(const struct VAO* vao) {
  glDeleteVertexArrays(vao->size, &vao->id);
}

#endif

