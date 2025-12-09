#include "curl_llm.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Escape special characters for JSON
void escape_json_string(const char *input, char *output, size_t output_size) {
  size_t j = 0;
  for (size_t i = 0; input[i] != '\0' && j < output_size - 2; i++) {
    if (input[i] == '\n') {
      output[j++] = ' '; // Replace newline with space
    } else if (input[i] == '\r') {
      // Skip carriage return
    } else if (input[i] == '"') {
      output[j++] = '\\';
      output[j++] = '"';
    } else if (input[i] == '\\') {
      output[j++] = '\\';
      output[j++] = '\\';
    } else {
      output[j++] = input[i];
    }
  }
  output[j] = '\0';
}

// a function for first reading the file using f_open
int validate_file(const char *path) {
  FILE *valid_or_not_file;
  errno_t err;
  // FIX1:- USING BINARY READ TO AVOID WINDOWS TEXT TRANSLATION ISSUE (\r\n ->
  // \n)
  err = fopen_s(&valid_or_not_file, path, "rb");
  if (err != 0) {
    perror("cant read file");
    return 0;
  }
  return 1;
}

// parsing and reading the output of the cli if error is present
FILE *compile_input_file(const char *path) {
  FILE *compile_test_c;
  if (validate_file(path)) {

    char command[256];

    // using snprintf to prevent buffer overflow
    snprintf(command, sizeof(command), "gcc -Wall -c %s 2>&1", path);
    // 2>&1 means right pipe fd 2(stderr) to 1 (stdout)[ stdout useful for cli
    // output] :)
    compile_test_c = _popen(command, "r");
    if (compile_test_c == NULL) {
      perror("_popen failed");
      exit(1);
    }
  }
  return compile_test_c;
}
// first taking the input from the user using argv
// argv vector fetches the input provided by user from the command line
int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: ./input <file.c>\n");
    exit(1);
  }
  const char *source_file = argv[1];
  FILE *pipe_fp;
  char buffer[128];

  pipe_fp = compile_input_file(source_file);

  if (pipe_fp == NULL) {
    return EXIT_FAILURE;
  }

  printf("Executing command for file: %s\n", source_file);

  // Read and store the output from the compiler (errors/warnings)
  char error_output[2048] = "";
  while (fgets(buffer, sizeof(buffer), pipe_fp) != NULL) {
    printf("%s", buffer);
    size_t offset = strlen(error_output);
    if (offset < sizeof(error_output) - 1) {
      snprintf(error_output + offset, sizeof(error_output) - offset, "%s",
               buffer);
    }
  }

  // Build JSON payload for Groq API with escaped error output
  char escaped_error[4096];
  escape_json_string(error_output, escaped_error, sizeof(escaped_error));

  char json_payload[8192];
  snprintf(json_payload, sizeof(json_payload),
           "{\"model\":\"llama-3.3-70b-versatile\",\"messages\":[{\"role\":"
           "\"user\",\"content\":\"Fix this C code error: %s\"}]}",
           escaped_error);

  run_llm_request(json_payload);
  // Close the pipe and wait for the child process to finish
  if (_pclose(pipe_fp) == -1) {
    perror("_pclose failed");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

  return 0;
}
