// src/server.c
#include "server.h"
#include "config.h"
#include "http.h"
#include "http_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int server_init(server_context *ctx) {
  int opt = 1;
  ctx->kq = kqueue();

  if ((ctx->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failure");
    return -1;
  }

  if (setsockopt(ctx->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt");
    return -1;
  }

  ctx->address.sin_family = AF_INET;
  ctx->address.sin_addr.s_addr = INADDR_ANY;
  ctx->address.sin_port = htons(PORT);

  if (bind(ctx->server_fd, (struct sockaddr *)&ctx->address,
           sizeof(ctx->address)) < 0) {
    perror("bind failed");
    return -1;
  }

  if (listen(ctx->server_fd, 3) < 0) {
    perror("listen error");
    return -1;
  }

  EV_SET(&ctx->change, ctx->server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
  kevent(ctx->kq, &ctx->change, 1, NULL, 0, NULL);

  return 0;
}

static void handle_new_connection(server_context *ctx, socklen_t addrlen) {
  int new_socket =
      accept(ctx->server_fd, (struct sockaddr *)&ctx->address, &addrlen);
  if (new_socket < 0)
    return;

  EV_SET(&ctx->change, new_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
  kevent(ctx->kq, &ctx->change, 1, NULL, 0, NULL);
}

static void handle_client_data(int client_fd, char *request_buffer,
                               size_t *buffer_pos) {
  ssize_t valread = read(client_fd, request_buffer + *buffer_pos,
                         MAX_REQUEST_SIZE - *buffer_pos - 1);

  if (valread <= 0) {
    close(client_fd);
    *buffer_pos = 0;
    memset(request_buffer, 0, MAX_REQUEST_SIZE);
    return;
  }

  *buffer_pos += valread;
  request_buffer[*buffer_pos] = '\0';

  if (strstr(request_buffer, "\r\n\r\n")) {
    DEBUG_LOG("Complete request received: %s", request_buffer);
    char *response = http_handler(request_buffer, *buffer_pos);
    send(client_fd, response, strlen(response), 0);

    *buffer_pos = 0;
    memset(request_buffer, 0, MAX_REQUEST_SIZE);
  } else {
    DEBUG_LOG("Partial request: %s", request_buffer);
  }
}

static void handle_events(server_context *ctx, int nev, char *request_buffer,
                          size_t *buffer_pos) {
  socklen_t addrlen = sizeof(ctx->address);

  for (int i = 0; i < nev; i++) {
    if (ctx->events[i].ident == (uintptr_t)ctx->server_fd) {
      handle_new_connection(ctx, addrlen);
    } else {
      handle_client_data(ctx->events[i].ident, request_buffer, buffer_pos);
    }
  }
}

int server_run(server_context *ctx) {
  char request_buffer[MAX_REQUEST_SIZE] = {0};
  size_t buffer_pos = 0;

  while (1) {
    int nev = kevent(ctx->kq, NULL, 0, ctx->events, MAX_EVENTS, NULL);
    if (nev > 0) {
      handle_events(ctx, nev, request_buffer, &buffer_pos);
    }
  }
  return 0;
}
