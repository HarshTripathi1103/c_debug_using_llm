#include "../curl/curl.h"
#include "env_parser.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define a struct to store the full growing response
typedef struct memory {
  char *response; // pointer to allocated buffer that will hold merged chunks
  size_t size;    // number of bytes currently stored in response
} CallBackStruct;

// as soon as the response is recived
// libcurl will call this function
// This callback function gets called by libcurl as soon as there is data
// received that needs to be saved. For most transfers, this callback gets
// called many times and each invoke delivers another chunk of data. ptr points
// to the delivered data, and the size of that data is nmemb; size is always 1.

/*
 * @params
 * *data -> New chunk received from network
 * size -> Size of one element (most 1 FOR HTTP)
 * refer:-[https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html]
 * nmemb -> number of elements recived in this chunk (byte count if size=1)
 * *clientp -> Pointer we passed while setting CURLOPT_WRITEDATA, points to
 * struct memory
 * **/
static size_t cb(char *data, size_t size, size_t nmemb, void *clientp) {
  size_t realsize = nmemb; // it generally size*nmemb but as size is 1 at each
                           // iteration so only nmemb

  // to access mem convert the clientp to type memory
  CallBackStruct *mem = (CallBackStruct *)clientp;

  // expand previous buffer to hold old data +new chunk + null terminator
  char *ptr = realloc(mem->response, mem->size + realsize + 1);

  if (!ptr) {
    return 0;
  }

  // Save the newly allocated memory block into struct for future appends
  mem->response = ptr;

  // Copy the new chunk and append it exactly after old sotred bytes
  memcpy(&(mem->response[mem->size]), data, realsize);

  // Update the size counter because we added more bytes
  mem->size += realsize;

  // add \0 at end so response becomes a valid C string
  mem->response[mem->size] = 0;

  // return number of bytes so libcurl knows it succeeded
  return realsize;
}

int main(void) {
  static const char *postthis = "{\
    \"model\": \"openai/gpt-oss-20b\", \
    \"input\": \"Explain the importance of fast language models\" \
  }";

  CURL *curl;

  CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
  if (res) {
    return (int)res;
  }

  char *post_url = parsed_variable(".env", "GROQ_LINK");
  char *groq_api = parsed_variable(".env", "GROQ_API");

  curl = curl_easy_init();

  // create a struct for headers
  struct curl_slist *list = NULL;
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, post_url);

    // set the callback func to handle writing the server response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    // tell curl to write data on stdout i.e. the terminal using the callback
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, stdout);

    // bearer token is passed as vairalbe
    // so using sprintf and buffer
    char buffer_length[256];

    // store all the static and var string in a buffer
    snprintf(buffer_length, sizeof(buffer_length), "Authorization: Bearer %s",
             groq_api);

    // list of headers
    list = curl_slist_append(list, buffer_length);

    list = curl_slist_append(list, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

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
