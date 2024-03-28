#ifndef VBO_H
#define VBO_H

// Vertex Buffer Object
struct VBO {
  GLuint id;
  GLsizei size;
};

struct VBO vboCreate(GLsizei size);

void vboBind(const struct VBO* self, const void* data, GLsizeiptr dataSize);
void vboUnbind(void);
void vboDelete(const struct VBO* self);

#endif

