#include "zsfs.h"
#include <fuse/fuse.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

FILE *super_fd, *file_fd;

int main(int argc, char **argv) {

  // arg handling
  // format for commands
  // zssfs <method> arguments
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
        perror("unable to open super: ");
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

  return fuse_main(2, new_argv, &op, NULL);
}

static int z_getattr(const char *restrict path, struct stat *restrict st) {

  // fill stat with info about file
  st->st_gid = getgid();
  st->st_uid = getuid();
  st->st_atime = time(NULL);
  st->st_mtime = time(NULL);

  // for now only implementing two files
  // will work on that later
  if (strcmp(path, "/") == 0) {
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2;
  } else {
    st->st_mode = S_IFREG | 0644;
    st->st_nlink = 1;
    st->st_size = 1024;
  }

  return 0;
}

static int z_readdir(const char *restrict path, void *buf,
                     fuse_fill_dir_t filler, off_t offset,
                     struct fuse_file_info *fi) {

  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);

  if (strcmp(path, "/") == 0) {

    filler(buf, "file1", NULL, 0);
    filler(buf, "file2", NULL, 0);
  }

  return 0;
}

static int z_read(const char *restrict path, char *buffer, size_t size,
                  off_t offset, struct fuse_file_info *fi) {

  char *file1 = "this is file one";
  char *file2 = "this is file two";

  if (strcmp(path, "/file1") == 0) {

    memcpy(buffer, file1 + offset, size);
    return strlen(file1) - offset;

  } else if (strcmp(path, "file2") == 0) {

    memcpy(buffer, file2 + offset, size);
    return strlen(file2) - offset;
  }

  return -1;
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
