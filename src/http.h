#pragma once
#include "config.h"

// HTTP Status Codes and Messages
#define HTTP_SUCCESS "HTTP/1.1 200 OK"
#define HTTP_BAD_REQUEST "HTTP/1.1 400 Bad Request"
#define HTTP_NOT_FOUND "HTTP/1.1 404 Not Found"
#define HTTP_METHOD_NOT_ALLOWED "HTTP/1.1 405 Method Not Allowed"
#define HTTP_INTERNAL_ERROR "HTTP/1.1 500 Internal Server Error"

// Common Headers
#define HTTP_CONTENT_TYPE "Content-Type"
#define HTTP_CONTENT_LEN "Content-Length"
#define HTTP_CONNECTION "Connection"
#define HTTP_DATE "Date"
#define HTTP_SERVER "Server"

// Content Types
#define CONTENT_TEXT_PLAIN "text/plain"
#define CONTENT_HTML "text/html"
#define CONTENT_JSON "application/json"

// Header size calculations
#define CRLF "\r\n"
#define MAX_STATUS_LINE_SIZE 64
#define DATE_HEADER_SIZE 37 // "Date: Wed, 21 Oct 2015 07:28:00 GMT\r\n"
#define HTTP_HEADERS_MAX_SIZE                                                  \
  (MAX_STATUS_LINE_SIZE + sizeof(HTTP_CONTENT_TYPE) + 32 +                     \
   sizeof(HTTP_CONTENT_LEN) + 16 + DATE_HEADER_SIZE + sizeof(HTTP_SERVER) +    \
   32 + 4) // Final \r\n\r\n

#define MAX_MESSAGE_SIZE (MAX_RESPONSE_SIZE - HTTP_HEADERS_MAX_SIZE)
#define MAX_REQUEST_SIZE (8 * 1024) // 8KB total request size
#define MAX_RESPONSE_SIZE 1024
#define MAX_HEADER_SIZE (4 * 1024)      // 4KB for headers
#define MAX_HEADERS 50                  // Maximum number of headers
#define MAX_URL_LENGTH 2048             // Common browser URL length limit
#define MAX_METHOD_LENGTH 8             // GET, POST, PUT, DELETE, etc
#define MAX_VERSION_LENGTH 16           // HTTP/1.1, HTTP/2.0
#define MAX_BODY_SIZE (1 * 1024 * 1024) // 1MB body size

#define MAX_RESPONSE_HEADERS 20
#define MAX_ERROR_MESSAGE 256 // For error responses

#define READ_BUFFER_SIZE 4096  // Socket read buffer size
#define WRITE_BUFFER_SIZE 4096 // Socket write buffer size

// Response function prototypes
char *success_res(char *buffer, const char *msg, const char *content_type);
char *bad_request_res(char *buffer, const char *msg, const char *content_type);
char *not_found_res(char *buffer);
char *method_not_allowed_res(char *buffer);
char *internal_error_res(char *buffer);
