#include "env_parser.h"

char *parsed_variable(const char *file_name, char *l_value) {
  // take left value and find corresponding right value and return it :]

  FILE *fp;
  errno_t err;
  char buffer[MAX_LINE_LENGTH];
  char *key_found = NULL;

  err = fopen_s(&fp, file_name, "rb");

  if (err != 0) {
    perror("cant read env file");
    exit(0);
  }

  // go line be line and find r_value which contains the l_value passed
  // read the file
  while (fgets(buffer, MAX_LINE_LENGTH, fp) != NULL) {

    // remove the trailing char \n by assiging \0 (0) to them
    buffer[strcspn(buffer, "\r\n")] = 0;

    // find '='
    char *equal_ptr = strchr(buffer, '=');

    if (equal_ptr != NULL) {
      // null terminate the key ptr temporarily
      *equal_ptr = '\0';
      char *key = buffer;
      char *value_start = equal_ptr + 1;

      if (strcmp(key, l_value) == 0) {

        // key found  then alloc mem
        key_found = malloc(strlen(value_start) + 1);
        if (key_found == NULL) {
          perror("cant allocate mem to found key");
          fclose(fp);
          exit(0);
        }
        strcpy(key_found, value_start);
        printf("%s \n", key_found);
        break;
      }
    }
  }

  fclose(fp);
  return key_found;
}
