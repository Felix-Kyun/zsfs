#include "../include/log/log.h"
#include "types.h"
#include "zsfs.h"
#include <stdio.h>
#include <stdlib.h>

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

  for (int i = 0; i < superHead.size; i++) {
    if (i == block_id) {

      uint bit_state;
      fread(&bit_state, sizeof(uint), 1, super_fd);

      return bit_state;
    } else
      fseek(super_fd, sizeof(uint), SEEK_CUR);
  }

  return -1;
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
