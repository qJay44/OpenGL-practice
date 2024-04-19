#include <stdio.h>
#include <string.h>

#include "utils.h"

char* readFile(const char* path, bool printContent) {
  FILE* fptr;
  u32 maxItems = DEFAULT_BUFFER_ITEMS;
  char* buffer = calloc(maxItems, sizeof(char) * maxItems);

  fopen_s(&fptr, path, "r");

  if (fptr) {
    u32 i = 0;

    if (printContent) {
      printf("\n====== File content (%s) ======\n", path);
      while (!feof(fptr)) {
        if (i == maxItems) {
          maxItems *= 2;
          arrResizeChar(&buffer, sizeof(char) * maxItems);
        }

        char ch = fgetc(fptr);
        buffer[i++] = ch;
        printf("%c", ch);
      }

      // Print lower border
      printf("\n=============================");
      int pathLen = strlen(path);
      while (pathLen--) printf("=");
      printf("\n\n");

    } else
      while (!feof(fptr)) {
        if (i == maxItems) {
          maxItems *= 2;
          arrResizeChar(&buffer, sizeof(char) * maxItems);
        }
        buffer[i++] = fgetc(fptr);
      }
    arrResizeChar(&buffer, sizeof(char) * i);
  } else
    printf("File can't be opened: %s\n", path);

  fclose(fptr);
  return buffer;
}

void arrResizeChar(char** arr, u32 newSize) {
  char* newArr = realloc(*arr, newSize);

  if (newArr)
    *arr = newArr;
  else {
    printf("arrayExpand realloc fail\n");
    exit(EXIT_FAILURE);
  }
}

void arrResizeFloat(float** arr, u32 newSize) {
  float* newArr = realloc(*arr, newSize);

  if (newArr)
    *arr = newArr;
  else {
    printf("arrayExpand realloc fail\n");
    exit(EXIT_FAILURE);
  }
}

void arrResizeUint(uint** arr, u32 newSize) {
  uint* newArr = realloc(*arr, newSize);

  if (newArr)
    *arr = newArr;
  else {
    printf("arrayExpand realloc fail\n");
    exit(EXIT_FAILURE);
  }
}

void arrResizeVec2s(vec2s** arr, u32 newSize) {
  vec2s* newArr = realloc(*arr, newSize);

  if (newArr)
    *arr = newArr;
  else {
    printf("arrayExpand realloc fail\n");
    exit(EXIT_FAILURE);
  }

}

void arrResizeVec3s(vec3s** arr, u32 newSize) {
  vec3s* newArr = realloc(*arr, newSize);

  if (newArr)
    *arr = newArr;
  else {
    printf("arrayExpand realloc fail\n");
    exit(EXIT_FAILURE);
  }

}

void arrResizeVec4s(vec4s** arr, u32 newSize) {
  vec4s* newArr = realloc(*arr, newSize);

  if (newArr)
    *arr = newArr;
  else {
    printf("arrayExpand realloc fail\n");
    exit(EXIT_FAILURE);
  }
}

void concat(const char* s1, const char* s2, char* out, rsize_t outSize) {
  strcpy_s(out, outSize, s1);
  strcat_s(out, outSize, s2);
}

