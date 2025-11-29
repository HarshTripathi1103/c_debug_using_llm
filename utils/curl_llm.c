#include "../curl/curl.h"
#include "env_parser.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// as soon as the response is recived
// libcurl will call this function
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {

  // cast the user data pointer to a FILE pointer(here stdout) :)
  return fwrite(contents, size, nmemb, (FILE *)userp);
}

int main(void) {
  static const char *postthis = "hello world";

  CURL *curl;

  CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
  if (res) {
    return (int)res;
  }

  char *webhook = parsed_variable(".env", "WEB_HOOK");
  char *post_url = parsed_variable(".env", "GROQ_LINK");
  char *groq_api = parsed_variable(".env", "GROQ_API");

  curl = curl_easy_init();

  // create a struct for headers
  struct curl_slist *list = NULL;
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, post_url);

    // bearer token is passed as vairalbe
    // so using sprintf and buffer
    char buffer_length[256];

    // store all the static and var string in a buffer
    snprintf(buffer_length, sizeof(buffer_length), "Authorization: Bearer %s",
             groq_api);

    // list of headers
    list = curl_slist_append(list, buffer_length);

    list = curl_slist_append(list, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
                     postthis); /* if we do not provide POSTFIELDSIZE, libcurl
                                   calls strlen() by itself */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));

    /* Perform the request, res gets the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return (int)res;
}
