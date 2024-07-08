#include "zsfs.h"
#include "../include/log/log.h"
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
  if ((super_fd == NULL) || (fs_fd == NULL)) {
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

  return init_fuse(new_argv);
}

void umount_handler(int code) {

  info("closing file");

  close_fd();

  return;
}

void zhelp() {
  char *help = "impl help";

  puts(help);
}
