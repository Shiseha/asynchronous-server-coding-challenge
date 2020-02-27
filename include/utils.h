#ifndef UTILS_H
#define UTILS_H

#define PORT 2826
#define MAX_CONNECTIONS 256
#define READ_SIZE 5

#include <stdbool.h>

typedef struct {
  int nb_write_left;
} client_context_t;

typedef struct {
  bool want_read;
  bool want_write;
} client_status_t;

void  terminate(char *msg);
void  close_epfd(int epfd);
int   initiate_socket();
void  non_blocking_socket(int socketfd);
int   accept_connection(int server_fd);

#endif /* UTILS_H */