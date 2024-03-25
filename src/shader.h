#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <stdlib.h>

[[nodiscard]]
GLint shaderLoad(const char* path, int type) {
  if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER) {
    printf("Shader load error: specified type is wrong\n");
    exit(1);
  }

  char shaderStr[1024];
  readFile(path, shaderStr, 1024, false);

  const char* ptrShaderStr = shaderStr;
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &ptrShaderStr, NULL);

  return shader;
}

void shaderCompile(GLuint shader) {
  GLint hasCompiled;
  char infoLog[1024];

  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);

  // if GL_FALSE
  if (!hasCompiled) {
    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    printf("\n===== Shader compilation error =====\n\n%s", infoLog);
    printf("====================================\n\n");
  }
}

void shaderProgramLink(GLuint program) {
  GLint hasLinked;
  char infoLog[1024];

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &hasLinked);

  // if GL_FALSE
  if (!hasLinked) {
    glGetProgramInfoLog(program, 1024, NULL, infoLog);
    printf("\n===== Shader link error =====\n\n%s", infoLog);
    printf("\n=============================\n\n");
  }
}

#endif

