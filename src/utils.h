#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

void readFile(const char* path, char* out, int outSize, int printContent) {
  FILE* fptr;

  fopen_s(&fptr, path, "r");

  if (fptr) {
    char ch;
    int i = 0;

    if (printContent) {
      printf("\n====== File content (%s) ======\n", path);
      while (!feof(fptr)) {
        if (i >= outSize) {
          printf("\nFile read error: File size excesses given char array\n");
          return;
        }
        ch = fgetc(fptr);
        out[i++] = ch;
        printf("%c", ch);
      }

      // Print lower border
      printf("\n=============================");
      int pathLen = strlen(path);
      while (pathLen--) printf("=");
      printf("\n\n");

    } else
      while (!feof(fptr)) {
        if (i >= outSize) {
          printf("\nFile read error: File size excesses given char array\n");
          return;
        }
        out[i++] = fgetc(fptr);
      }
  } else
    printf("File can't be opened: %s\n", path);

  fclose(fptr);
}

[[nodiscard]]
GLint loadShader(const char* path, int type, int printContent) {
  if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER) {
    printf("Shader load error: specified type is wrong\n");
    exit(1);
  }

  char shaderStr[1024];
  readFile(path, shaderStr, 1024, printContent);

  const char* ptrShaderStr = shaderStr;
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &ptrShaderStr, NULL);

  return shader;
}

