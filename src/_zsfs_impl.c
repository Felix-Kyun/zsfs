#include "../include/log/log.h"
#include "types.h"
#include "zsfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *super_fd = NULL, *fs_fd = NULL;
Super_head superHead;
Super_base superBase;
Fs FS;

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
  for (int i = 0; i < superHead.size; i++) {
    fread(&superBase.inode_data_bitmap + i, sizeof(int), 1, super_fd);
    fread(&superBase.free_bitmap + (i + superHead.size), sizeof(int), 1,
          super_fd);
  }

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

struct stat *get_inode_stat(inode_id_t id) {
  struct stat *st = calloc(1, sizeof(struct stat));

  return st;
}
