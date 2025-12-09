#include "curl_llm.h"
#include <stdlib.h>

size_t cb(char *data, size_t size, size_t nmemb, void *clientp) {
  size_t realsize = size * nmemb;
  CallBackStruct *mem = (CallBackStruct *)clientp;

  char *ptr = realloc(mem->response, mem->size + realsize + 1);
  if (!ptr) {
    return 0;
  }

  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
  mem->response[mem->size] = 0;
  return realsize;
}

int run_llm_request(const char *postthis) {
  CallBackStruct chunk;
  chunk.response = malloc(1); // Start with 1 byte
  chunk.size = 0;

  CURL *curl;

  CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
  if (res) {
    return (int)res;
  }

  char *post_url = parsed_variable(".env", "GROQ_LINK");
  char *groq_api = parsed_variable(".env", "GROQ_API");

  curl = curl_easy_init();

  struct curl_slist *list = NULL;
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, post_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    char buffer_length[256];
    snprintf(buffer_length, sizeof(buffer_length), "Authorization: Bearer %s",
             groq_api);

    list = curl_slist_append(list, buffer_length);
    list = curl_slist_append(list, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    // SEND postthis AS REQUEST BODY
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    } else {
      printf("\nLLM Response:\n%s\n", chunk.response);
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(list);
    free(chunk.response);
  }

  curl_global_cleanup();
  return (int)res;
}
