#ifndef __ZSFS_INTERNAL_TYPES_H
#define __ZSFS_INTERNAL_TYPES_H

#include <stdint.h>
#include <sys/types.h>

typedef unsigned int inode_id_t;
typedef unsigned char uchar;
typedef unsigned int count_t;
typedef unsigned int blkid_t;
typedef unsigned int blkcount_t;

// filesystem structures
// super head 64b
// padding is added to make it 64 bytes
typedef struct Super_head {
  char name[32]; // max 31 char file system name
  count_t size;
  count_t block_size;
  uchar __Padding[24]; // adjust it accordingly to align to 64 bytes
} Super_head;

// super head will me constructed according to the size in the size;
typedef struct Super_base {
  count_t *inode_data_bitmap;
  count_t *free_bitmap;
} Super_base;

typedef struct Fs {
  unsigned char *raw_data;
} Fs;

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

#endif
