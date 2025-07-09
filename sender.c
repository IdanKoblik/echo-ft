#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include "filechunk.pb-c.h"
#include "sender.h"

#define CHUNK_SIZE 1024

int send_chunk(int sock, struct sockaddr_in *dest, FileChunk *chunk) {
  size_t packed_size = file_chunk__get_packed_size(chunk);
  uint8_t *buffer = (uint8_t *)malloc(packed_size);
  file_chunk__pack(chunk, buffer);
  
  ssize_t sent = sendto(sock, buffer, packed_size, 0, (struct sockaddr *)dest, sizeof(*dest));
  if (sent < 0) {
    fprintf(stderr, "Cannot send data to peer\n");
    return -1;
  }
 
  printf("Sent %zd bytes: %s chunk (%d bytes of data)\n", sent, chunk->filename, chunk->size);
  free(buffer);
  return 0;
}

int send_file(const Config *cfg) {
  FILE* file = fopen(cfg->path, "rb");
  if (!file) {
    fprintf(stderr, "Cannot find file: %s\n", cfg->path);
    return -1;
  }

  char *filename = strrchr(cfg->path, '/');
  if (filename)
      filename++;
  else
    filename = cfg->path;

  fseek(file, 0, SEEK_END);
  long total_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  int sockfd;
  struct sockaddr_in dest;

  bzero(&dest, sizeof(dest));
  dest.sin_addr.s_addr = inet_addr(cfg->addr);
  dest.sin_port = htons(cfg->port);
  dest.sin_family = AF_INET;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
    fprintf(stderr, "Connection failed\n");
    fclose(file);
    return -1;
  }

  unsigned char buffer[CHUNK_SIZE];
  size_t bytes_read;

  while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
    FileChunk chunk = FILE_CHUNK__INIT;

    chunk.data.data = (uint8_t *)malloc(bytes_read);
    memcpy(chunk.data.data, buffer, bytes_read);
    chunk.data.len = bytes_read;

    chunk.version = (char *)"1"; // TODO
    chunk.filename = filename;
    chunk.size = (int)bytes_read;
    chunk.total_size = (int)total_size;

    send_chunk(sockfd, &dest, &chunk);
    free(chunk.data.data);
  }

  fclose(file);
  return 0;
}

