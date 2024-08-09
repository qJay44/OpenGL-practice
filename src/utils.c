#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "utils.h"

char* readFile(const char* path, bool printContent) {
  FILE* fptr;
  u32 itemsCount = 0xff;
  char* buffer = malloc(sizeof(char) * itemsCount + 1);

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
    size_t sz = sizeof(char) * i + 1;
    arrResizeChar(&buffer, sz, &sz);

    if (printContent) {
      printf("\n====== File content (%s) ======\n", path);
      printf("%s\n", buffer);
      printf("=============================");
      int pathLen = strlen(path);
      while (pathLen--) printf("=");
      printf("\n\n");
    }
  } else {
    printf("File can't be opened: %s\n", path);
    exit(EXIT_FAILURE);
  }

  fclose(fptr);
  return buffer;
}

byte* readFileBytes(const char* path, size_t* outSize) {
  FILE* fptr;
  size_t size = 0;
  u32 itemsCount = 0xff;
  byte* buffer = malloc(sizeof(byte) * itemsCount);

  fopen_s(&fptr, path, "rb");

  if (fptr) {
    u32 i = 0;

    while (!feof(fptr)) {
      if (i == itemsCount) {
        size = sizeof(byte) * itemsCount;
        arrResizeByte(&buffer, size, &size);
        itemsCount = size / sizeof(byte);
      }
      buffer[i++] = fgetc(fptr);
    }

    // Set the exact size of the array //

    size = sizeof(byte) * i;
    byte* newArr = malloc(size);

    for (int j = 0; j < i; j++)
      newArr[j] = buffer[j];

    free(buffer);
    buffer = newArr;

    /////////////////////////////////////
  } else
    printf("File can't be opened: %s\n", path);

  fclose(fptr);
  *outSize = size;
  return buffer;

}

char* getFileNameFromPath(const char* path) {
  char* fn;
  if (path == NULL)
    printf("(getFileNameFromPath) the given path is NULL\n");
  (fn = strrchr(path, '/')) ? ++fn : (fn = (char*)path);

  return fn;
}

char* getExtensionFromFileName(const char* path) {
  char* ext;
  if (path == NULL)
    printf("(getExtensionFromFileName) the given path is NULL\n");
  (ext = strrchr(path, '.')) ? ++ext : (ext = (char*)path);

  return ext;
}

int randBetween(int min, int max) {
  return rand() % (max + 1 - min) + min;
}

bool listDirectoryContents(const char *sDir) {
  WIN32_FIND_DATA fdFile;
  HANDLE hFind = NULL;

  char sPath[2048];

  //Specify a file mask. *.* = We want everything!
  sprintf(sPath, "%s\\*.*", sDir);

  if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) {
    printf("Path not found: [%s]\n", sDir);
    return false;
  }

  do {
    //Find first file will always return "."
    //    and ".." as the first two directories.
    if(strcmp(fdFile.cFileName, ".") != 0
    && strcmp(fdFile.cFileName, "..") != 0) {
      //Build up our file path using the passed in
      //  [sDir] and the file/foldername we just found:
      sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);

      //Is the entity a File or Folder?
      if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        printf("Directory: %s\n", sPath);
        listDirectoryContents(sPath); //Recursion, I love it!
      }
      else{
        printf("File: %s\n", sPath);
      }
    }
  } while(FindNextFile(hFind, &fdFile)); //Find the next file.

  FindClose(hFind); //Always, Always, clean things up!

  return true;
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

void arrResizeVersors(versors** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  versors* newArr = malloc(*outNewSize);

  for (int i = 0; i < oldSize / sizeof(versors); i++)
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

void arrResizeByte(byte** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  byte* newArr = malloc(sizeof(byte) * *outNewSize);

  for (int i = 0; i < oldSize / sizeof(byte); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

void arrResizeChar(char** arr, size_t oldSize, size_t* outNewSize) {
  *outNewSize = oldSize * 2;
  char* newArr = malloc(sizeof(char) * *outNewSize);

  for (int i = 0; i < oldSize / sizeof(char); i++)
    newArr[i] = (*arr)[i];

  free(*arr);
  *arr = newArr;
}

