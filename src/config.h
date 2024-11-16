#pragma once
#define MAX_EVENTS 10
#define PORT 8080
#define DEBUG_MODE 0

#if DEBUG_MODE
#define DEBUG_LOG(fmt, ...)                                                    \
  do {                                                                         \
    fprintf(stdout, "[DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__,            \
            __VA_ARGS__ + 0);                                                  \
    fflush(stdout);                                                            \
  } while (0)

#define DEBUG_LOG_SIMPLE(msg)                                                  \
  do {                                                                         \
    fprintf(stdout, "[DEBUG] %s:%d: %s\n", __FILE__, __LINE__, msg);           \
    fflush(stdout);                                                            \
  } while (0)
#else
#define DEBUG_LOG(fmt, ...) ((void)0)
#define DEBUG_LOG_SIMPLE(msg) ((void)0)
#endif

#define LOG_INFO(msg) fprintf(stdout, "[INFO] %s\n", msg)
#define LOG_ERROR(msg) fprintf(stderr, "[ERROR] %s\n", msg)
