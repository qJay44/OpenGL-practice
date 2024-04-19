#include <stdio.h>
#include <stdlib.h>

#include "shader.h"

GLuint load(const char* path, int type) {
  char* shaderStr = readFile(path, false);
  const char* shaderStrPtr = shaderStr;
  GLuint shaderId = glCreateShader(type);
  glShaderSource(shaderId, 1, &shaderStrPtr, NULL);

  free(shaderStr);
  return shaderId;
}

GLint compile(const char* path, int type) {
  GLint shaderId = load(path, type);

  GLint hasCompiled;
  char infoLog[1024];

  glCompileShader(shaderId);
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &hasCompiled);

  // if GL_FALSE
  if (!hasCompiled) {
    glGetShaderInfoLog(shaderId, 1024, NULL, infoLog);
    printf("\nShader: %s\n", path);
    printf("\n===== Shader compilation error =====\n\n%s", infoLog);
    printf("====================================\n\n");
  }

  return shaderId;
}

void link(GLuint program) {
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

GLint shaderCreate(const char* vsPath, const char* fsPath) {
  GLint vertexShaderId = compile(vsPath, GL_VERTEX_SHADER);
  GLint fragmentShaderId = compile(fsPath, GL_FRAGMENT_SHADER);

  GLint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShaderId);
  glAttachShader(shaderProgram, fragmentShaderId);
  link(shaderProgram);

  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);

  return shaderProgram;
}

