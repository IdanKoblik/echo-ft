#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "filechunk.pb-c.h"
#include <inttypes.h>
#include "receiver.h"

#define BUFFER_SIZE 2048

int receive_file(const Config *cfg) {
    struct sockaddr_in receiver_addr, sender_addr;
  socklen_t addr_len = sizeof(sender_addr);

  uint8_t buffer[BUFFER_SIZE];
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "Cannot create socket\n");
    return -1;
  }

  memset(&receiver_addr, 0, sizeof(receiver_addr));
  receiver_addr.sin_family = AF_INET;
  receiver_addr.sin_addr.s_addr = inet_addr(cfg->addr);
  receiver_addr.sin_port = htons(cfg->port);

  if (bind(sockfd, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0) {
    fprintf(stderr, "Cannot bind to sender\n");
    return -1;
  }

  FILE *file = NULL;
  printf("Receiver listening on port %d...\n", cfg->port);

  int total = 0;
  while (1) {
    ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&sender_addr, &addr_len);
    if (recv_len < 0) {
      fprintf(stderr, "Cannot receive data\n");
      break;
    }

    FileChunk *chunk = file_chunk__unpack(NULL, recv_len, buffer);
    if (chunk == NULL) {
      fprintf(stderr, "Failed to unpack received data\n");
      break;
    }

    if (file == NULL) {
        file = fopen(chunk->filename, "wb");
      if (!file) {
        fprintf(stderr, "Cannot open file to write: %s\n", chunk->filename);
        file_chunk__free_unpacked(chunk, NULL);
        continue;
      }
      printf("Receiving file: %s\n", chunk->filename);
    }

    size_t written = fwrite(chunk->data.data, 1, chunk->data.len, file);
    printf("fwrite wrote %zu bytes\n", written);
    if (written != chunk->data.len) {
      fprintf(stderr, "Warning: fwrite wrote fewer bytes than expected!\n");
    } else printf("Received chunk (%d bytes)\n", chunk->size);

    total += chunk->data.len;
    if (total >= chunk->total_size) {
      printf("File transfer complete.\n");
      file_chunk__free_unpacked(chunk, NULL);
      break;
    }

    file_chunk__free_unpacked(chunk, NULL);
  }

  printf("good\n");
  if (file)
    fclose(file);

  close(sockfd);
  return 0;
}

