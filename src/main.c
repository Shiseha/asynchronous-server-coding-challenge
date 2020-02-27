#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "utils.h"

#define PORT 2826
#define MAX_CONNECTIONS 256

void		  close_epfd(int epfd)
{
	if (close(epfd) == -1)
  {
    fprintf(stderr, "Failed to close epoll fd\n");
    exit(EXIT_FAILURE);
  }
}

void      exit_error(char* err_msg)
{
  fprintf(stderr, "%s\n", err_msg);
  exit(EXIT_FAILURE);
}


int       main()
{
  int     port = PORT_NUMBER;
  int     event_count;
  size_t  bytes_read;
  printf("Serving on %d\n", port);

  struct  epoll_event event, events[MAX_CONNECTIONS];
  int     epfd = epoll_create1(0);

  if (epfd == -1)
  {
    fprintf(stderr, "Failed to create epoll fd\n");
    exit(EXIT_FAILURE);
  }

  event.data.fd = 0;
  event.events = EPOLLIN;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &event))
  {
    fprintf(stderr, "Failed to add fd to epoll\n");
    close_epfd(epfd);
  }

  while(1)
  {
    event_count = epoll_wait(epfd, events, MAX_CONNECTIONS, -1);
    for (int i = 0; i < event_count; i++)
    {
      printf("%d ready events\n", event_count);
      bytes_read = read(events[i].data.fd, read_buffer, 10);
      printf("%zd bytes read\n", bytes_read);
      read_buffer[bytes_read] = '\0';
      printf("%s\n", read_buffer);
    }
  }

  close_epfd(epfd);
  return 0;
}