CC = gcc
CFLAGS = -Wall
OUT = zsfs

build: zssfs.c 
	$(CC) zssfs.c -o $(OUT) $(CFLAGS)  `pkg-config fuse --cflags --libs`
