#include <stdio.h>
#include "config.h"
#include "sender.h"
#include "receiver.h"

int main(int argc, char *argv[]) {
  Config *cfg = parse_config(argc, argv);
  if (!cfg) {
    fprintf(stderr, "Cannot parse flags\n");
    return -1;
  }

  if (cfg->mode == RECEIVER) {
    if (receive_file(cfg) < 0) {
      fprintf(stderr, "Cannot receive file\n");
      return -1;
    }
  } else if (cfg->mode == SENDER) {
    if (send_file(cfg) < 0) {
      fprintf(stderr, "Cannot send file [%s]\n", cfg->path);
      return -1;
    }
  } else {
    fprintf(stderr, "Unknown mode\n");
    return -1;
  }

  destroy_config(cfg);
  return 0;
}
