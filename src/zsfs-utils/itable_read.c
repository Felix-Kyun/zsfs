#include "../../include/log/log.h"
#include "../zsfs.h"
#include <stdlib.h>

// command format: itable_new super.bin inode_id blkid

int main(int argc, char **argv) {

  open_super(argv[1]);
  read_super_head();

  load_inode_table();

  Log(LOG_INFO, "blkid of inode %d: %d\n", atoi(argv[2]),
      get_block_from_inode(atoi(argv[2])));

  write_inode_table();

  close_fd();
  return 0;
}
