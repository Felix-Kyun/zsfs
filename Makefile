CC = gcc
CFLAGS = -Wall
BIN_DIR = bin
SRC_DIR = src
UTILS_DIR = $(SRC_DIR)/zsfs-utils

build: 
	$(CC) $(SRC_DIR)/zsfs.c -o $(BIN_DIR)/zsfs $(CFLAGS)  `pkg-config fuse --cflags --libs`

utils:
	@echo "Building utils"
	@cd $(UTILS_DIR) && $(MAKE) all

clean: 
	@echo "Cleaning build files"
	@rm -rf ./bin/*
