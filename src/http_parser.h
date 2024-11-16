#pragma once
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
  HTTP_GET,
  HTTP_POST,
  HTTP_PUT,
  HTTP_DELETE,
  HTTP_UNKNOWN
} http_method;

typedef struct {
  http_method method;
  char path[1024];
  char version[16];
  char host[256];
  char content_type[128];
  size_t content_length;
  char body[4096];
} http_request;

bool parse_http_request(const char *raw_request, size_t length,
                        http_request *request);

http_method parse_method(const char *method_str);
bool parse_request_line(const char *line, http_request *request);
bool parse_header(const char *header_line, http_request *request);
char *http_handler(const char *request_buffer, size_t len);
