#ifndef EBO_H
#define EBO_H

// Element Buffer Object
struct EBO {
  GLuint id;
  GLsizei size;
};

struct EBO eboCreate(GLsizei size, const void* data, GLsizeiptr dataSize);

void eboBind(const struct EBO* self);
void eboUnbind(void);
void eboDelete(const struct EBO* self);

#endif

