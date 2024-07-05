// Felix
// ./zsfs_super_alloc ./path/to/super.bin name size

#include "../types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// typedef struct Super_base {
//   int *inode_data_bitmap;
//   int *free_bitmap;
// } Super_base;

#include "../zsfs.h"

int main(int argc, char **argv) {

  if (argc != 4) {
    perror("invalid number or arguments: ");
    exit(-1);
  }

  super_fd = fopen(argv[1], "wb");
  if (super_fd == NULL) {
    perror("unable to open the super file");
    exit(-1);
  }

  strcpy(superHead.name, argv[2]);
  superHead.size = atoi(argv[3]);
  printf("creating zsfs with name %s\n", superHead.name);
  printf("initializing to size: %d\n", superHead.size);

  fwrite(&superHead, sizeof(struct Super_head), 1, super_fd);

  // write the data/indoe and free bitmap
  unsigned int zero = 0;
  for (int i = 0; i < 2 * superHead.size; i++)
    fwrite(&zero, sizeof(int), 1, super_fd);

  fclose(super_fd);

  return 0;
}
