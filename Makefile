CC=clang
CFLAGS=-I./src -Wall -Wextra -Werror -pedantic -std=c11
LDFLAGS=

SRC_DIR=src
INC_DIR=include
OBJ_DIR=obj

SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

DEBUG_FLAGS=-g -O0 -fsanitize=address,undefined
RELEASE_FLAGS=-O2

all: server

debug: CFLAGS+=$(DEBUG_FLAGS)
debug: LDFLAGS+=$(DEBUG_FLAGS)
debug: server

release: CFLAGS+=$(RELEASE_FLAGS)
release: server

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

server: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

clean:
	rm -rf $(OBJ_DIR) server

.PHONY: all debug release clean
