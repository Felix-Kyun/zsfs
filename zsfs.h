#ifndef __ZSSFS_HEADER__
#define __ZSSFS_HEADER__
#define FUSE_USE_VERSION 30
#pragma pack(1)

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>

// help function
void zhelp();

void umount_handler(int code);

// file system image methods 

// super
static int zsuper_allocate();
static int zsuper_open();

// main data storage file 
static int zfile_allocate();
static int zfile_open();

// file system access methods 

// getattr
static int z_getattr(const char *path, struct stat *st);

// readdir 
static int z_readdir(const char *path, void *buf,
                      fuse_fill_dir_t filler, off_t offset,
                      struct fuse_file_info *fi);

// read 
static int z_read(const char *path, char *buffer, size_t size,
                   off_t offset, struct fuse_file_info *fi);

// defination for struct fuse_operation
struct fuse_operations op = {
    .getattr = z_getattr, .readdir = z_readdir, .read = z_read};
#endif
