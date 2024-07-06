#include "../../include/log/log.h"
#include "../zsfs.h"
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 3) {
    Log(LOG_ERROR, "Invalid amount of arguments");
    exit(-1);
  }

  open_super(argv[1]);
  read_super_head();

  find_free_blk(atoi(argv[2]));

  close_fd();
  return 0;
}
