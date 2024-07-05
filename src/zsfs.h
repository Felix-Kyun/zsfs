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

// internal
#include "types.h"

#define BLOCK_SIZE 1024

// filesystem structures
typedef struct Super_head {
  char name[32]; // max 31 char file system name
  unsigned int size;
} Super_head;

// super head will me constructed according to the size in the size;
typedef struct Super_base {
   int *inode_data_bitmap;
   int *free_bitmap;
} Super_base;


typedef struct Fs {
  unsigned char *raw_data;
} Fs;

/*
 * the things we need now are
 * [x] inode
 * [ ] methods to read inodes itself
 * [ ] methods to read data from fs according to inode
 * [ ] dentry in inodes (i have no idea how to store dentry as its variable length)
 */

// inode struct 
// contains standard members + some internal members which is used inside zsfs
// if the inode is a dir then dentry is used which is written after inode in the space allocated to inode 
// |--block-||--------||--------||--------||--------||-------| 
// +---------------------------------------------------------|
// | inode block                                             |
// | +--------------+---------------------------------+      |
// | | inode struct | dentry_entry[inode.dentry_size] |      |
// | +--------------+---------------------------------+      |
// +---------------------------------------------------------|
// some of the members are disabled until implemeted
// till then it with be calculated according to the user and their request
// with getuid(), getgid(), time(NULL)
typedef struct inode {

  // standard members 
  mode_t mode; 
  nlink_t nlink;
  // uid_t user_id;
  // gid_t group_id;
  blksize_t st_blksize; 
  blkcnt_t  st_blocks;  
  // time_t atime;
  // time_t mtime;
  // time_t ctime;

  // internal code 
  inode_id_t inode_id;
  // uchar is_dir;
  count_t children_count;
  count_t inode_size;
  count_t blocks_used;
  count_t dentry_size;

  // actual data info
  blkid_t data_blk;
  blkcount_t blk_count;

} inode;

typedef struct dentry_item {
  char name[32];
  int inode_id;
} dentry_item;

typedef struct dentry {
  dentry_item *entry;
} dentry;

// find a free block 
// finds the first free block 
// whose consecutive blocks are enough to store $size
// uses first fit algo
blkid_t find_free_blk(size_t size);

// methods to read the inode 

// get block number from the inode id
// basically just goes over inode_data_bitmap to find the correct inode
// if not found then return -1
// @i inode struct 
int block_from_inode(int i);

// is it a directory ?
#define isdir(inode) (inode.mode & S_IFDIR)

// read inode $id and store its content to $i
int read_inode(inode_id_t id, inode *i);

// reads $size data from $offset given the inode
int read_data(inode i, size_t size, off_t offset);

// fills up inode i from path 
// this shit is complicated 
// so basically we need to navigate one by one to the final file
// this may or maynot take multiple calls to read_inode() 
// can be dont with a loop or recursion
int read_inode_from_path(const char* path, inode *i);

// write stuff
// write a inode to fs
int read_inode(inode i);


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

// misc functions

// shows a simple help message
void zhelp();

// singal handler
// aparrently fuse doesnt signal on umount for whatever reason
// atleast i dont know the signal 
// PS: it doesnt do anything 
// INFO: its useless just like you :p
void umount_handler(int code);


#endif

