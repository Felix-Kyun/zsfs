#include "zsfs.h"
#include <fuse/fuse.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

FILE *super_fd, *file_fd;
Super_head superHead;
Super_base superBase;
Fs FS;

int main(int argc, char **argv) {

  // arg handling
  // format for commands
  // zssfs flags <MountPoint>
  // use zhelp for help

  int c;

  while ((c = getopt(argc, argv, "hs:f:")) != -1) {
    switch (c) {
    case 's':
      if ((super_fd = fopen(optarg, "rb+")) == NULL) {
        perror("unable to open super: ");
        return 1;
      }
      break;
    case 'f':
      if ((file_fd = fopen(optarg, "rb+")) == NULL) {
        perror("unable to open fs file: ");
        return 1;
      }
      break;
    case '?':
    case 'h':
      zhelp();
      return 1;
      break;
    }
  }

  // check if the files were opened correctly
  if ((super_fd == NULL) || (file_fd == NULL)) {
    perror("missing super/file");
    zhelp();
    return 1;
  }

  // set signal handler
  struct sigaction my_signal_handler = {.sa_flags = 0,
                                        .sa_handler = umount_handler};
  sigemptyset(&my_signal_handler.sa_mask);
  sigaction(SIGTERM, &my_signal_handler, NULL);

  // extract the file name and pass onto
  char *new_argv[] = {argv[0], argv[optind]};

  // init fs super structures
  fread(&superBase, sizeof(struct Super_head), 1, super_fd);

  // init the base
  // // alloc space and init to 0
  superBase.inode_data_bitmap = (int *)calloc(superHead.size, sizeof(int));
  superBase.free_bitmap = (int *)calloc(superHead.size, sizeof(int));

  // read state from file
  for (int i = 0; i < superHead.size; i++) {
    fread(&superBase.inode_data_bitmap + i, sizeof(int), 1, super_fd);
    fread(&superBase.free_bitmap + (i + superHead.size), sizeof(int), 1,
          super_fd);
  }

  return fuse_main(2, new_argv, &op, NULL);
}

void umount_handler(int code) {

  puts("closing file");

  fclose(super_fd);
  fclose(file_fd);

  return;
}

void zhelp() {
  char *help = "impl help";

  puts(help);
}
