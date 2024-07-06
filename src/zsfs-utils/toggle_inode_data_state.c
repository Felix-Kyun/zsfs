#include "../../include/log/log.h"
#include "../types.h"
#include "../zsfs.h"
#include <stdlib.h>

// usage: toogle_block_state ./super.bin 100
int main(int argc, char **argv) {

  if (argc != 3) {
    err("invalid arg list");
    return -1;
  }

  Log(LOG_INFO, "Toggling inode/data state of block %d", atoi(argv[2]));

  open_super(argv[1]);

  info("Reading superBlock");
  read_super_head();

  Log(LOG_INFO, "superblock blocks: %d", superHead.size);

  int data_or_inode = read_inode_or_data(atoi(argv[2]));
  if (data_or_inode == 0) {
    Log(LOG_INFO, "current state is data block");
    Log(LOG_INFO, "changing to inode");
    write_inode_or_data(atoi(argv[2]), 1);
  } else {
    Log(LOG_INFO, "current state is inode block");
    Log(LOG_INFO, "changing to data");
    write_inode_or_data(atoi(argv[2]), 0);
  }

  close_fd();
}
