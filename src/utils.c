#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include "utils.h"

void  terminate(char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

void  non_blocking_socket(int socketfd)
{
  int  status = fcntl(socketfd, F_SETFL,
                  fcntl(socketfd, F_GETFL, 0) | O_NONBLOCK);

  if (status == -1)
  {
    terminate("Fcntl error");
  }
}

int   initiate_socket()
{
  int socketfd;
  struct sockaddr_in  server_addr;

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);


  if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    terminate("Initiate socket error");
  }

  if (bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
  {
    terminate("Bind error");
  }

  if (listen(socketfd, SOMAXCONN) == -1)
  {
    terminate("Listen error");
  }

  non_blocking_socket(socketfd);

  return socketfd;
}