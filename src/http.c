#include "http.h"
#include <stdio.h>
#include <string.h>

static char *format_http_res(char *buffer, const char *status,
                             const char *content_type, const char *msg) {
  size_t msg_len = strlen(msg);
  if (msg_len > MAX_MESSAGE_SIZE) {
    return NULL;
  }

  int written = snprintf(
      buffer, MAX_RESPONSE_SIZE, "%s\r\n%s: %s\r\n%s: %zu\r\n\r\n%s\r\n",
      status, HTTP_CONTENT_TYPE, content_type, HTTP_CONTENT_LEN, msg_len, msg);
  if (written < 0 || written >= MAX_RESPONSE_SIZE) {
    return NULL;
  }

  return buffer;
}

char *success_res(char *buffer, const char *msg, const char *content_type) {
  return format_http_res(buffer, HTTP_SUCCESS, content_type, msg);
}

char *bad_request_res(char *buffer, const char *msg, const char *content_type) {
  return format_http_res(buffer, HTTP_BAD_REQUEST, content_type, msg);
}
