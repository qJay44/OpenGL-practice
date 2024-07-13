#include <stdio.h>
#include <string.h>

#include "utils.h"

char* readFile(const char* path, bool printContent) {
  FILE* fptr;
  u32 itemsCount = 0xff;
  char* buffer = malloc(sizeof(char) * itemsCount);

  fopen_s(&fptr, path, "r");

  if (fptr) {
    u32 i = 0;

    while (!feof(fptr)) {
      if (i == itemsCount) {
        size_t sz = sizeof(char) * itemsCount;
        arrResizeChar(&buffer, sz, &sz);
        itemsCount = sz / sizeof(char);
      }
      buffer[i++] = fgetc(fptr);
    }
    // Crop at the last element
    size_t sz = sizeof(char) * i;
    arrResizeChar(&buffer, sz, &sz);

    if (printContent) {
      printf("\n====== File content (%s) ======\n", path);
      printf("%s\n", buffer);
      printf("=============================");
      int pathLen = strlen(path);
      while (pathLen--) printf("=");
      printf("\n\n");
    }
  } else
    printf("File can't be opened: %s\n", path);

  fclose(fptr);
  return buffer;
}

byte* readFileBytes(const char* path) {
  FILE* fptr;
  u32 itemsCount = 0xff;
  byte* buffer = malloc(sizeof(byte) * itemsCount);

  fopen_s(&fptr, path, "rb");

  if (fptr) {
    u32 i = 0;

    while (!feof(fptr)) {
      if (i == itemsCount) {
        size_t sz = sizeof(byte) * itemsCount;
        arrResizeByte(&buffer, sz, &sz);
        itemsCount = sz / sizeof(byte);
      }
      buffer[i++] = fgetc(fptr);
    }

    // Crop at the last element
    size_t sz = sizeof(byte) * i;
    arrResizeByte(&buffer, sz, &sz);
  } else
    printf("File can't be opened: %s\n", path);

  fclose(fptr);
  return buffer;

}

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
void int2str(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';

    // Reverse the string
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
}

void arrResizeFloat(float** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  float* newArr = malloc(*outNewSize);

  for (int i = 0; i < oldSize / sizeof(float); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void arrResizeUint(u32** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  u32* newArr = malloc(*outNewSize);

  for (int i = 0; i < oldSize / sizeof(u32); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void arrResizeVec2s(vec2s** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  vec2s* newArr = malloc(*outNewSize);

  for (int i = 0; i < oldSize / sizeof(vec2s); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void arrResizeVec3s(vec3s** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  vec3s* newArr = malloc(*outNewSize);

  for (int i = 0; i < oldSize / sizeof(vec3s); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void arrResizeVec4s(vec4s** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  vec4s* newArr = malloc(*outNewSize);

  for (int i = 0; i < oldSize / sizeof(vec4s); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void arrResizeMat4s(mat4s** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  mat4s* newArr = malloc(*outNewSize);

  for (int i = 0; i < oldSize / sizeof(mat4s); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void arrResizeObject(Object** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  Object* newArr = malloc(*outNewSize);

  for (int i = 0; i < oldSize / sizeof(Object); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void arrResizeTexture(Texture** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  Texture* newArr = malloc(*outNewSize);

  for (int i = 0; i < oldSize / sizeof(Texture); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void arrResizeChar(char** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  char* newArr = malloc(*outNewSize);

  for (int i = 0; i < oldSize / sizeof(char); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void arrResizeCharPtr(char*** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  char** newArr = malloc(sizeof(char) * *outNewSize);

  for (int i = 0; i < oldSize / sizeof(char); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void arrResizeByte(byte** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  byte* newArr = malloc(sizeof(byte) * *outNewSize);

  for (int i = 0; i < oldSize / sizeof(byte); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void concat(const char* s1, const char* s2, char* out, rsize_t outSize) {
  strcpy_s(out, outSize, s1);
  strcat_s(out, outSize, s2);
}

