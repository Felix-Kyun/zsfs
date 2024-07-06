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

  Log(LOG_INFO, "Toggling free_state of block %d", atoi(argv[2]));

  open_super(argv[1]);

  info("Reading superBlock");
  read_super_head();

  Log(LOG_INFO, "superblock blocks: %d", superHead.size);

  uint free_state = read_free_state(atoi(argv[2]));
  Log(LOG_INFO, "current bit state is %d", free_state);

  uint new_state = (free_state == 0) ? 1 : 0;
  Log(LOG_INFO, "new bit state is %d", new_state);
  write_free_state(atoi(argv[2]), new_state);

  close_fd();
}
