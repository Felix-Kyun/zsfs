CC = gcc
CFLAGS = -Wall
OUT = bin/zsfs

build: zsfs.c 
	$(CC) zsfs.c -o $(OUT) $(CFLAGS)  `pkg-config fuse --cflags --libs`
