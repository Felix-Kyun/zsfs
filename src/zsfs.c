#include "zsfs.h"
#include "./_zsfs_impl.c"
#include <fuse/fuse.h>
#include <getopt.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv) {

  // arg handling
  // format for commands
  // zssfs flags <MountPoint>
  // use zhelp for help

  int c;

  while ((c = getopt(argc, argv, "hs:f:")) != -1) {
    switch (c) {
    case 's':
      open_super(optarg);
      break;
    case 'f':
      open_fs(optarg);
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
  read_super_head();

  // init the base
  read_super_base();

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

// INFO: perf can be improved by skipping the already searched blocks
blkid_t find_free_blk(size_t size) {

  blkcount_t required_count = ceil(((float)size) / BLOCK_SIZE);

  // navigate to super base
  fseek(super_fd, sizeof(struct Super_head), SEEK_SET);

  // loop over free_bitmap till the requested size is found
  int current_bit_state;
  int saved_cursor_position;
  for (int i = 0; i < superHead.size; i++) {
    // read the current bitmap state
    fread(&current_bit_state, sizeof(int), 1, super_fd);

    if (current_bit_state == 0) {

      // save current location
      saved_cursor_position = ftell(super_fd);

      // check for consecutive bits
      int longest_hit = 1;
      int inner_bit_state;
      for (int i = saved_cursor_position; i < superHead.size; i++) {
        fread(&inner_bit_state, sizeof(int), 1, super_fd);

        // if 0 then inc longest_hit else break;
        if (inner_bit_state == 0)
          longest_hit++;
        else {
          fseek(super_fd, saved_cursor_position, SEEK_SET);
          break;
        }
      }

      if (longest_hit >= size)
        return current_bit_state;
    }
  }

  return 0;
}
