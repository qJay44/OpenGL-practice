#ifndef RBO_H
#define RBO_H

struct RBO {
  GLuint id;
  GLsizei size;
};

[[nodiscard]]
struct RBO rboCreate(GLsizei size, GLenum targetType);

void rboBind(const struct RBO* self);
void rboUnbind(void);
void rboDelete(const struct RBO* self);

#endif

