#include "server.h"
#include <stdlib.h>

int main(void) {
  server_context ctx;
  if (server_init(&ctx) != 0)
    return EXIT_FAILURE;
  return server_run(&ctx);
}
