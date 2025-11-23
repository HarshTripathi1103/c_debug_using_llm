#define __STDC_WANT_LIB_EXT1__ 1
#define _GNU_SOURCE
#include <corecrt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// first taking the input from the user using argv
// argv vector fetches the input provided by user from the command line
int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: ./input <file.c>\n");
    exit(1);
  }
  // if the correct input is given by the user read the argv[1] input that is
  // the file passed by the user
  FILE *debugfile;
  errno_t err;

  // FIX1:- USING BINARY READ TO AVOID WINDOWS TEXT TRANSLATION ISSUE (\r\n ->
  // \n)
  err = fopen_s(&debugfile, argv[1], "rb");
  if (err != 0) {
    printf("Error while reading the file provided: %s\n", argv[1]);
    exit(1);
  }
  // read the size of the file
  // first move the file pointer to end using fseek
  if (fseek(debugfile, 0L, SEEK_END) != 0) {
    perror("fseek to end failed");
    fclose(debugfile);
    exit(1);
  }
  long fsize = ftell(debugfile);
  if (fsize == -1L) {
    perror("ftell failed");
    fclose(debugfile);
    exit(1);
  }
  // move the file pointer to top again
  if (fseek(debugfile, 0L, SEEK_SET) != 0) {
    perror("fseek to start failed");
    fclose(debugfile);
    exit(1);
  }
  // allocate a buffer of the size of file plus '\0'(null operator)
  char *code = malloc(fsize + 1);
  if (!code) {
    perror("malloc error");
    fclose(debugfile);
    exit(1);
  }
  // read the actual contents
  size_t bytes_read = fread(code, 1, (size_t)fsize, debugfile);
  if (bytes_read != (size_t)fsize) { // Verify full read
    printf("Partial read: %zu of %ld bytes\n", bytes_read, fsize);
    free(code);
    fclose(debugfile);
    exit(1);
  }
  // at the last byte assign '\0'
  code[fsize] = '\0';
  fclose(debugfile);

  // Print only if successful
  printf("Read %ld bytes:\n%s\n", fsize, code);
  free(code); // Safe now
  return 0;
}
