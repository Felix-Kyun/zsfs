#include "../../include/log/log.h"
#include "../types.h"
#include <stdio.h>
#include <stdlib.h>

// usage: toogle_block_state ./super.bin 100
int main(int argc, char **argv) {

  if (argc != 3) {
    err("invalid arg list");
    return -1;
  }

  Log(LOG_INFO, "Toggling free_state of block %d", atoi(argv[2]));

  FILE *super_fd = fopen(argv[1], "rb+");
  rewind(super_fd);

  Super_head base;
  info("Reading superBlock");
  fread(&base, sizeof(Super_head), 1, super_fd);

  if (base.size < atoi(argv[2])) {
    Log(LOG_ERROR, "Requested block %d but fs max is %d", atoi(argv[2]),
        base.size);
    goto close;
  }

  Log(LOG_INFO, "superblock blocks: %d", base.size);

  for (int i = 0; i < base.size; i++) {
    if (i == atoi(argv[2])) {
      Log(LOG_INFO, "hit, reading from %d", ftell(super_fd));

      unsigned int current_state;
      long saved_pos = ftell(super_fd);
      fread(&current_state, sizeof(unsigned int), 1, super_fd);

      Log(LOG_INFO, "current bit state is %d", current_state);
      // fseek(super_fd, -1 * (sizeof(unsigned int)), SEEK_CUR);
      fseek(super_fd, saved_pos, SEEK_SET);

      int new_state = (current_state == 0) ? 1 : 0;
      Log(LOG_INFO, "writing %d to %d", new_state, ftell(super_fd));
      fwrite(&new_state, sizeof(unsigned int), 1, super_fd);

    } else
      fseek(super_fd, sizeof(unsigned int), SEEK_CUR);
  }

  // Log(LOG_INFO, "Current cursor position: %d", ftell(super_fd));
close:
  fclose(super_fd);
  return 0;
}
