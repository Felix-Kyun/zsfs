// Felix
// ./zsfs_super_alloc ./path/to/super.bin name size

#include "../types.h"
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

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
  fwrite(&zero, sizeof(int), 2 * superHead.size, super_fd);

  Inode_table_header header = {.count = 0};
  fwrite(&header, sizeof(struct Inode_table_header), 1, super_fd);

  // writing root inode
  load_inode_table();
  write_new_inode(0, 0);
  write_inode_table();

  inode root = {
      .mode = 0777 | S_IFDIR,
      .ctime = time(NULL),
      .mtime = time(NULL),
      .atime = time(NULL),
      .nlink = 2,
      .inode_size = sizeof(struct inode),
      .children_count = 2,
      .dentry_size = 2,
  };

  fclose(super_fd);

  return 0;
}
