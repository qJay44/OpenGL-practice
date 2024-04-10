#include <stdio.h>
#include <string.h>

#include "utils.h"

void readFile(const char* path, char* out, int outCount, int printContent) {
  FILE* fptr;

  fopen_s(&fptr, path, "r");

  if (fptr) {
    char ch;
    int i = 0;

    if (printContent) {
      printf("\n====== File content (%s) ======\n", path);
      while (!feof(fptr)) {
        if (i >= outCount) {
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
        if (i >= outCount) {
          printf("\nFile read error: File size excesses given char array\n");
          return;
        }
        out[i++] = fgetc(fptr);
      }
  } else
    printf("File can't be opened: %s\n", path);

  fclose(fptr);
}

void concat(const char* s1, const char* s2, char* out, rsize_t outSize) {
  strcpy_s(out, outSize, s1);
  strcat_s(out, outSize, s2);
}

