#pragma once
#include <curl/curl.h>
#include "env_parser.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct memory {
  char *response;
  size_t size;
} CallBackStruct;

size_t cb(char *data, size_t size, size_t nmemb, void *clientp);

int run_llm_request(const char *postthis);
