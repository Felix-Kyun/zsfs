#include "../include/log/log.h"
#include "types.h"
#include "zsfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

FILE *super_fd = NULL, *fs_fd = NULL;
Super_head superHead;
Super_base superBase;
Fs FS;
Inode_table itable;

#ifdef __ZSFS_MAIN
struct fuse_operations op = {
    .getattr = _zsfs_getattr,
    .readdir = _zsfs_readdir,
    .read = _zsfs_read,
};

int init_fuse(char **new_argv) { return fuse_main(2, new_argv, &op, NULL); }
#endif

int open_super(const char *path) {
  if ((super_fd = fopen(path, "rb+")) == NULL) {
    err("unable to open super file");
    return -1;
  }

  return 0;
}

int open_fs(const char *path) {
  if ((fs_fd = fopen(path, "rb+")) == NULL) {
    err("unable to open fs file");
    return 1;
  }

  return 0;
}

int read_super_head() {
  rewind(super_fd);

  // read the super base
  fread(&superHead, sizeof(struct Super_head), 1, super_fd);

  return 0;
}

int read_super_base() {

  superBase.inode_data_bitmap = (uint *)calloc(superHead.size, sizeof(uint));
  superBase.free_bitmap = (uint *)calloc(superHead.size, sizeof(uint));

  // read state from file
  fread(superBase.inode_data_bitmap, sizeof(int), superHead.size, super_fd);
  fread(superBase.free_bitmap, sizeof(int), superHead.size, super_fd);

  return 0;
}

int read_free_state(blkid_t block_id) {

  if (block_id > superHead.size) {
    err("block id exceeds the fs block length");
    close_fd();
    exit(-1);
  }

  goto_base_start();

  // nav
  fseek(super_fd, sizeof(uint) * block_id, SEEK_CUR);

  uint bit_state;
  fread(&bit_state, sizeof(uint), 1, super_fd);

  return bit_state;
}

int goto_free_bitmap(blkid_t block_id) {
  if (block_id > superHead.size) {
    err("block id exceeds the fs block length");
    close_fd();
    exit(-1);
  }

  goto_base_start();

  fseek(super_fd, block_id * sizeof(uint), SEEK_CUR);

  return 0;
}

int write_free_state(blkid_t block_id, uint state) {
  if (block_id > superHead.size) {
    err("block id exceeds the fs block length");
    close_fd();
    exit(-1);
  }

  goto_free_bitmap(block_id);

  fwrite(&state, sizeof(uint), 1, super_fd);

  return 0;
}

count_t get_block_size() { return superHead.block_size; }

void close_fd() {
  if (super_fd != NULL) {
    fclose(super_fd);
    super_fd = NULL;
  }

  if (fs_fd != NULL) {
    fclose(fs_fd);
    fs_fd = NULL;
  }
}

blkid_t find_free_blk(size_t size) {

  blkcount_t longest_hit = 0;
  blkid_t start_block = 0;
  for (int i = 0; i < FS_SIZE; i++) {
    if (read_free_state(i) == 0) {
      if (longest_hit == 0)
        start_block = i;
      longest_hit++;
    } else {
      longest_hit = 0;
    }

    if (longest_hit * BLOCK_SIZE >= size) {
      Log(LOG_SUCCESS, "Found free blocks that can store %d blocks at %d", size,
          start_block);
      return start_block;
    }
  }

  return 0;
}

int write_inode_or_data(blkid_t block_id, uint mode) {

  if (block_id > superHead.size) {
    err("block id exceeds the fs block length");
    close_fd();
    exit(-1);
  }

  goto_inode_data_bmap();

  // nav to that block
  fseek(super_fd, sizeof(uint) * block_id, SEEK_CUR);

  fwrite(&mode, sizeof(uint), 1, super_fd);

  return 0;
}

uint read_inode_or_data(blkid_t block_id) {

  if (block_id > superHead.size) {
    err("block id exceeds the fs block length");
    close_fd();
    exit(-1);
  }

  goto_inode_data_bmap();

  // nav to that block
  fseek(super_fd, sizeof(uint) * block_id, SEEK_CUR);

  uint state;
  fread(&state, sizeof(uint), 1, super_fd);

  return state;
}

int load_inode_table() {

  // goto inode table location
  goto_itable();

  Inode_table_header header;
  fread(&header, sizeof(struct Inode_table_header), 1, super_fd);

  itable.count = header.count;

  itable.enteries = (struct Inode_table_entry *)calloc(
      itable.count, sizeof(struct Inode_table_entry));

  fread(itable.enteries, sizeof(struct Inode_table_entry), itable.count,
        super_fd);

  return 0;
}

int write_inode_table() {

  // goto inode table location
  goto_itable();

  struct Inode_table_header header = {.count = itable.count};

  // write the header containing all the enteries
  fwrite(&header, sizeof(struct Inode_table_header), 1, super_fd);

  // write all the enteries
  fwrite(itable.enteries, sizeof(struct Inode_table_entry), itable.count,
         super_fd);

  return 0;
}

int write_new_inode(inode_id_t id, blkid_t bid) {
  itable.count++;

  itable.enteries = (struct Inode_table_entry *)realloc(
      itable.enteries, sizeof(struct Inode_table_entry) * itable.count);

  (itable.enteries[itable.count - 1]).id = id;
  (itable.enteries[itable.count - 1]).bid = bid;

  return 0;
}

blkid_t get_block_from_inode(inode_id_t id) {

  // linear search inside the table
  for (int i = 0; i < itable.count; i++) {
    if ((itable.enteries[i]).id == id)
      return (itable.enteries[i]).bid;
  }

  return 0;
}

int modify_block_id(inode_id_t id, blkid_t new_bid) {

  //  linear search the inode id
  for (int i = 0; i < itable.count; i++) {
    if ((itable.enteries[i]).id == id) {

      // do ur work here
      (itable.enteries[i]).bid = new_bid;
    }
  }

  return 0;
}

int read_inode(inode_id_t id, inode *i) {

  blkid_t bid = get_block_from_inode(id);

  fseek(fs_fd, bid, SEEK_SET);
  fread(i, sizeof(struct inode), 1, fs_fd);

  return 0;
}

struct stat *get_inode_stat(inode_id_t id) {
  struct stat *st = calloc(1, sizeof(struct stat));

  inode i;
  read_inode(id, &i);

  // timing stuff
  st->st_atime = i.atime;
  st->st_ctime = i.ctime;
  st->st_mtime = i.mtime;

  st->st_blksize = BLOCK_SIZE;
  st->st_blocks = i.blocks_used;

  st->st_gid = getgid();
  st->st_uid = getuid();

  st->st_mode = i.mode;
  st->st_nlink = i.nlink;
  st->st_size = i.data_size;

  return st;
}

inode_id_t write_inode(inode i) {}

#ifdef __ZSFS_MAIN

int _zsfs_getattr(const char *path, struct stat *st) {

  // check if its root
  if (strcmp(path, "/") == 0) {

    struct stat *inode_stat = get_inode_stat(0);

    // memcpy(st, inode_stat, sizeof(struct stat));
    *st = *inode_stat;

  } else {
    // the path is not root so transverse it accordingly
  }

  return 0;
}

int _zsfs_readdir(const char *path, void *buffer, fuse_fill_dir_t filler,
                  off_t offset, struct fuse_file_info *fi) {

  if (strcmp(path, "/") == 0) {

    // root dir

    // read dentry and add files accordingly
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);

  } else {

    // handle the cases of other dir here
  }

  return 0;
}

int _zsfs_read(const char *path, char *buffer, size_t size, off_t offset,
               struct fuse_file_info *fi) {

  return 0;
}

#endif
