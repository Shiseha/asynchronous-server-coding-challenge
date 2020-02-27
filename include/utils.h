#ifndef UTILS_H
#define UTILS_H

#define PORT 2826
#define MAX_CONNECTIONS 256
#define READ_SIZE 5

void  terminate(char *msg);
int   initiate_socket();

#endif /* UTILS_H */