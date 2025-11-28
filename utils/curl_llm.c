#include "../curl/curl.h"
#include "env_parser.h"
#include <stdio.h>
#include <string.h>

int main(void) {
  static const char *postthis = "hello world";

  CURL *curl;

  CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
  if (res) {
    return (int)res;
  }

  char *webhook = parsed_variable(".env", "WEB_HOOK");

  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, webhook);
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
