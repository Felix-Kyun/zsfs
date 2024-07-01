#ifndef __ZSSFS_HEADER__
#define __ZSSFS_HEADER__
#define FUSE_USE_VERSION 30
#pragma pack(1)

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BLOCK_SIZE 256

// filesystem structures
typedef struct Super_head {
  char name[32]; // max 31 char file system name
  unsigned int size;
} Super_head;

// super head will me constructed according to the size in the size;
// skel
typedef struct Super_base {
   int *inode_data_bitmap;
   int *free_bitmap;
} Super_base;
// #define SUPER_BASE(size) struct { \
//   int inode_data_bitmap[size];  \
//   int free_bitmap[size]; \
// } 

typedef struct Fs {
  unsigned char *raw_data;
} Fs;
//
// #define FS(size) struct { \
//   unsigned char raw_data[size]; \
// }

// help function
void zhelp();

void umount_handler(int code);

// file system image methods

// super
static int zsuper_open();

// main data storage file
static int zfile_open();

// file system access methods

// getattr
static int z_getattr(const char *path, struct stat *st);

// readdir
static int z_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                     off_t offset, struct fuse_file_info *fi);

// read
static int z_read(const char *path, char *buffer, size_t size, off_t offset,
                  struct fuse_file_info *fi);

// defination for struct fuse_operation
struct fuse_operations op = {
  .getattr = z_getattr,
  .read = z_read,
  .readdir = z_readdir,
};
#endif
