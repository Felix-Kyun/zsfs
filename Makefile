CC = gcc
CFLAGS = -Wall -D__ZSFS_MAIN
BIN_DIR = bin
SRC_DIR = src
UTILS_DIR = $(SRC_DIR)/zsfs-utils
INC_SRC = $(SRC_DIR)/log.c $(SRC_DIR)/_zsfs_impl.c

build: 
	$(CC) $(SRC_DIR)/zsfs.c -o $(BIN_DIR)/zsfs $(CFLAGS)  `pkg-config fuse --cflags --libs` $(INC_SRC)

utils:
	@echo "Building utils"
	@cd $(UTILS_DIR) && $(MAKE) all

clean: 
	@echo "Cleaning build files"
	@rm -rf ./bin/*
