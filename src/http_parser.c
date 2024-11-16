#include "http_parser.h"
#include "http.h"
#include "string.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// GET / HTTP/1.1
// Host: localhost:8080
// Connection: keep-alive
// sec-ch-ua: "Not A(Brand";v="24", "Chromium";v="110"
// sec-ch-ua-mobile: ?0
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7)
// Accept:
// text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8
// Sec-Fetch-Site: none
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9
//

http_method parse_method(const char *method_str) {
  if (!method_str)
    return HTTP_UNKNOWN;
  if (strcmp(method_str, "GET") == 0)
    return HTTP_GET;
  if (strcmp(method_str, "POST") == 0)
    return HTTP_POST;
  if (strcmp(method_str, "PUT") == 0)
    return HTTP_PUT;
  if (strcmp(method_str, "DELETE") == 0)
    return HTTP_DELETE;
  return HTTP_UNKNOWN;
}

bool parse_request_line(const char *line, http_request *request) {
  // make a 16 byte string
  char method[16] = {0};

  int matched = sscanf(line, "%15s %1023s HTTP/%15s", method, request->path,
                       request->version);

  if (matched != 3)
    return false;

  request->method = parse_method(method);
  return request->method != HTTP_UNKNOWN;
}

bool parse_header(const char *header_line, http_request *request) {
  char name[128] = {0};
  char value[1024] = {0};

  // is there colon?
  const char *colon = strchr(header_line, ':');
  if (!colon)
    return false;

  // colon is a pointer to the colon so we get the len here
  size_t name_len = colon - header_line;
  if (name_len >= sizeof(name))
    return false;

  // copy the name length part of the header to name
  strncpy(name, header_line, name_len);

  // copy everything after the colon ignore spaces
  const char *value_start = colon + 1;
  while (*value_start && isspace(*value_start))
    value_start++;

  size_t value_len = strlen(value_start);
  if (value_len >= sizeof(value))
    return false;

  strcpy(value, value_start);

  // now we figure out what type of header it is
  if (strcasecmp(name, "Host") == 0) {
    strncpy(request->host, value, sizeof(request->host) - 1);
  } else if (strcasecmp(name, HTTP_CONTENT_TYPE) == 0) {
    strncpy(request->content_type, value, sizeof(request->content_type) - 1);
  } else if (strcasecmp(name, HTTP_CONTENT_LEN) == 0) {
    request->content_length = atoi(value);
  }

  return true;
}

bool parse_http_request(const char *raw_request, size_t length,
                        http_request *request) {
  if (!raw_request || !request || length == 0)
    return false;
  // clear the memory location
  memset(request, 0, sizeof(*request));

  // max line length
  char line[4096];
  // starting pos
  size_t pos = 0;
  size_t line_start = 0;
  // flags
  bool first_line = true;
  bool headers_done = false;

  // let's go
  while (pos < length) {
    // if we are at the end of the line let's update some properties
    if (pos + 1 < length && raw_request[pos] == '\r' &&
        raw_request[pos + 1] == '\n') {
      size_t line_len = pos - line_start;
      // make sure we don't overflow
      if (line_len > sizeof(line) - 1)
        return false;

      // copy all the line data to line and set the final character to '\0'
      memcpy(line, raw_request + line_start, line_len);
      line[line_len] = '\0';

      // if we are at the end of the headers let's check if there is a body and
      // note the length
      if (line_len == 0) {
        headers_done = true;
        if (request->content_length > 0 && pos + 2 < length) {
          size_t body_len = length - (pos + 2);
          if (body_len > sizeof(request->body) - 1)
            return false;
          memcpy(request->body, raw_request + pos + 2, body_len);
          request->body[body_len] = '\0';
        }
        break;
      }

      if (first_line) {
        // if we can't parse the first line and we are on the first line just
        // return false
        if (!parse_request_line(line, request))
          return false;
        // mark first line as parsed
        first_line = false;
      } else {
        // parse the rest of the headers
        if (!parse_header(line, request))
          return false;
      }

      pos += 2;
      line_start = pos;
    } else {
      pos++;
    }
  }

  return !first_line && headers_done;
}

char *http_handler(const char *request_buffer, size_t len) {
  static char response_buffer[MAX_RESPONSE_SIZE];
  http_request req = {0};

  if (!parse_http_request(request_buffer, len, &req)) {
    bad_request_res(response_buffer, "invalid format", CONTENT_TEXT_PLAIN);
  }

  // TODO do more things
  switch (req.method) {
  case HTTP_GET:
    if (strcmp(req.path, "/") == 0) {
      return success_res(response_buffer, "Hello from server",
                         CONTENT_TEXT_PLAIN);
    }
    break;
  default:
    return bad_request_res(response_buffer, "invalid format",
                           CONTENT_TEXT_PLAIN);
  }

  return bad_request_res(response_buffer, "invalid format", CONTENT_TEXT_PLAIN);
}
