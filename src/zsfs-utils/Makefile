CC = gcc
CFLAGS = -Wall
INC_SRC = ../log.c
SRC_DIR = .
BIN_DIR = ../../bin
SOURCES = $(wildcard $(SRC_DIR)/*.c)
TARGETS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(SOURCES))

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(BIN_DIR)/%: $(SRC_DIR)/%.c
	@echo "Building " $(patsubst $(BIN_DIR)/%,%,$@)
	@$(CC) -o $@ $< $(CFLAGS) $(INC_SRC)

all: $(TARGETS)

clean: 
	@rm -rf $(BiN_DIR)/*

.PHONY: all clean
