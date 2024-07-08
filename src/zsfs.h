#ifndef __ZSSFS_HEADER__
#define __ZSSFS_HEADER__
#define FUSE_USE_VERSION 30
#pragma pack(1)

// #include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// internal
#include "types.h"

#define BLOCK_SIZE 1024

// vars for storing the data
extern FILE *super_fd, *fs_fd;
extern Super_head superHead;
extern Super_base superBase;
extern Fs FS;

/*
 * the things we need now are
 * [x] inode
 * [ ] methods to read inodes itself
 * [ ] methods to read data from fs according to inode
 * [ ] dentry in inodes (i have no idea how to store dentry as its variable length)
 */

// opens the super fd file 
int open_super(const char* path);

// opens the fs fd fie
int open_fs(const char* path);

void close_fd();

// reads super head state to superHead
int read_super_head();

// reads the super base state to superBase
// avoid using if possible
// as it uses a large amount of memory
int read_super_base();

// reads if a particular block is free or not
int read_free_state(blkid_t block_id);

// writes a state of a bit to free bitmap
int write_free_state(blkid_t block_id, uint state);

// macro to go to the start of super base
#define goto_base_start() fseek(super_fd, sizeof(struct Super_head), SEEK_SET)
#define goto_inode_data_bmap() fseek(super_fd, sizeof(struct Super_head) + (sizeof(uint) * superHead.size), SEEK_SET)

// used to navigate the free bitmap
int goto_free_bitmap(blkid_t block_id);

// get the block size of the fs from superhead
// #define get_block_size() superHead.block_size
count_t get_block_size();

#define BLOCK_COUNT superHead.size
#define FS_SIZE superHead.size

// find a free block 
// finds the first free block 
// whose consecutive blocks are enough to store $size
// uses first fit algo
blkid_t find_free_blk(size_t size);

// sets a blocks as either data block (0) or inode block(1)
int write_inode_or_data(blkid_t block_id, uint mode);

// query if a block is inode or data 
uint read_inode_or_data(blkid_t block_id);

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

// writes an inode to the fs
// INFO: for writing root inode use write_root_inode();
inode_id_t write_inode(inode i);

// writes a root inode
int write_root_inode(inode i);

// gets the stat of a inode 
struct stat *get_inode_stat(inode_id_t id);
// file system access methods

// methods to work with inode table
#define goto_itable() fseek(super_fd, \
        sizeof(struct Super_head) + 2 * (sizeof(unsigned int) * superHead.size), \
        SEEK_SET);

// loads the inode table into memory
int load_inode_table();

// write the inode table to the super.bin
int write_inode_table();

// append a new entry to the inode table
// used when creating a new inode
int write_new_inode(inode_id_t id, blkid_t bid);

// to get the block id from a given inode id
// used when reading inodes
blkid_t get_block_from_inode(inode_id_t id);

// used to update the blkid when a inode is rewritten to a new location
int modify_block_id(inode_id_t id, blkid_t new_bid);

// getattr
// static int z_getattr(const char *path, struct stat *st);

// readdir
// static int z_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
//                      off_t offset, struct fuse_file_info *fi);

// read
// static int z_read(const char *path, char *buffer, size_t size, off_t offset,
//                   struct fuse_file_info *fi);

// defination for struct fuse_operation
// struct fuse_operations op = {
//   .getattr = z_getattr,
//   .read = z_read,
//   .readdir = z_readdir,
// };

// misc functions

// shows a simple help message
void zhelp();

// singal handler
// aparrently fuse doesnt signal on umount for whatever reason
// atleast i dont know the signal 
// PS: it doesnt do anything 
// INFO: its useless just like you :p
void umount_handler(int code);

// fuse methods
int init_fuse(char **new_argv);
#ifdef __ZSFS_MAIN

#include<fuse/fuse.h>

// simply retuns the op struct

int _zsfs_getattr(const char* path, struct stat *st);
int _zsfs_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi );
int _zsfs_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi );

#endif

#endif

