CC = gcc
CFLAGS = -Wall
OUT = zsfs

build: zsfs.c 
	$(CC) zsfs.c -o $(OUT) $(CFLAGS)  `pkg-config fuse --cflags --libs`
