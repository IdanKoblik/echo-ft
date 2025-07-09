#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  SENDER,
  RECEIVER,
  UNKNOWN
} Mode;

typedef struct {
  Mode mode;
  char *path;
  char *addr;
  int port;
} Config;

Mode parse_mode(const char *arg);

Config* parse_config(int argc, char *argv[]);
int destroy_config(Config *cfg);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_H 
