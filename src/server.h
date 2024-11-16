#pragma once
#include "config.h"
#include <netinet/in.h>
#include <sys/event.h>

typedef struct {
  int kq;
  int server_fd;
  struct sockaddr_in address;
  struct kevent change;
  struct kevent events[MAX_EVENTS];
} server_context;

int server_init(server_context *ctx);
int server_run(server_context *ctx);
