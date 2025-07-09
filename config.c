#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "config.h"
#include <stdlib.h>

#define MAX_PORT 65535
#define MIN_PORT 0

Mode parse_mode(const char *arg) {
  if (strcmp(arg, "sender") == 0)
    return SENDER;

  if (strcmp(arg, "receiver") == 0)
    return RECEIVER;

  return UNKNOWN;
}

Config* parse_config(int argc, char *argv[]) {
  Config *cfg = (Config *)malloc(sizeof(Config));
  if (!cfg) {
    fprintf(stderr, "Cannot malloc Config\n");
    return NULL;
  }
  
  cfg->addr = NULL;
  cfg->path = NULL;
  cfg->mode = UNKNOWN;
  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--mode=", 7) == 0) {
      const char *val = argv[i] + 7;
      cfg->mode = parse_mode(val);
      if (cfg->mode == UNKNOWN) {
        fprintf(stderr, "Unknown mode: %s\n", val);
        free(cfg);
        return NULL;
      }
    } else if (strncmp(argv[i], "--port=", 7) == 0) {
      int port = atoi(argv[i] + 7);
      if (port <= MIN_PORT || port >= MAX_PORT) {
        fprintf(stderr, "Invalid port\n");
        free(cfg);
        return NULL;
      }
      cfg->port = port;
    } else if (strncmp(argv[i], "--addr=", 7) == 0) {
      cfg->addr = strdup(argv[i] + 7);
    } else if (strncmp(argv[i], "--path=", 7) == 0) {
      cfg->path = strdup(argv[i] + 7);
    } else {
      fprintf(stderr, "Invalid flag see manual page\n");
      free(cfg);
      return NULL;  
    }
  }

  return cfg;
}

int destroy_config(Config *cfg) {
  if (!cfg) 
    return -1;

  if (cfg->path)
    free(cfg->path);
  
  if (cfg->addr)
    free(cfg->addr);
  
  free(cfg);
  return 0;
}
