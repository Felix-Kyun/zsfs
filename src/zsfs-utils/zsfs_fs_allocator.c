// Felix
// ./zsfs_fs_alloc ./path/to/fs.bin block_count

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../zsfs.h"

// for now hardcoded block size
// 1024 means 4 char or an int
#define BLOCK_SIZE 1024

int main(int argc, char **argv) {

  if (argc != 3) {
    perror("invalid number or arguments: ");
    exit(-1);
  }

  fs_fd = fopen(argv[1], "wb");
  if (fs_fd == NULL) {
    perror("unable to open the fs file");
    exit(-1);
  }

  printf("Allocating fs with %d blocks of size %d\n", atoi(argv[2]),
         BLOCK_SIZE);
  void *empty_block =
      calloc(BLOCK_SIZE / sizeof(unsigned char), sizeof(unsigned char));

  for (int i = 0; i < atoi(argv[2]); i++) {
    fwrite(empty_block, BLOCK_SIZE, 1, fs_fd);
  }

  fclose(fs_fd);

  return 0;
}
