// Felix
// ./zsfs_super_info ./path/to/super.bin

#include "../types.h"
#include "../zsfs.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

  if (argc != 2) {
    perror("invalid number or arguments: ");
    exit(-1);
  }

  super_fd = fopen(argv[1], "rb");
  if (super_fd == NULL) {
    perror("unable to open the super file");
    exit(-1);
  }

  fread(&superHead, sizeof(struct Super_head), 1, super_fd);

  printf("FS name: %s\n", superHead.name);
  printf("FS size: %d\n", superHead.size);

  int bitmap_state;
  printf("inode/data bitmap (0/1)");
  for (int i = 0; i < superHead.size; i++) {
    if (i % 8 == 0)
      printf("\n");
    // printf("\nfile cursor pos: %ld\n", ftell(super_fd));
    fread(&bitmap_state, sizeof(int), 1, super_fd);
    printf("%d\t", bitmap_state);
  }

  printf("\nfree bitmap (0/1)");
  for (int i = 0; i < superHead.size; i++) {
    if (i % 8 == 0)
      printf("\n");
    fread(&bitmap_state, sizeof(int), 1, super_fd);
    printf("%d\t", bitmap_state);
  }

  fclose(super_fd);

  return 0;
}
