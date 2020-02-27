#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include "utils.h"
#include "poem.h"

client_context_t all_state[MAX_CONNECTIONS];

const client_status_t client_read = { .want_read = true, .want_write = false};
const client_status_t client_write = { .want_read = false, .want_write = true};
const client_status_t client_noevent = { .want_read = false, .want_write = false};

client_status_t   initate_context(int socketfd)
{
  client_context_t* client_context = &all_state[socketfd];
  client_context->nb_write_left = 0;

  return client_read;
}

client_status_t   read_request(int socketfd)
{
  client_context_t* client_context = &all_state[socketfd];

  char buff[READ_SIZE];

  int msg_size = read(socketfd, buff, sizeof(buff));
  if (msg_size < 0)
  {
    if (errno = EAGAIN || errno == EWOULDBLOCK)
      return client_read;
    else
      terminate("read failed");
  }
  client_context->nb_write_left = atoi(buff);
  if (client_context->nb_write_left == 0)
  {
    return client_noevent;
  }
  return client_write;
}

client_status_t   write_request(int socketfd)
{
  client_context_t* client_context = &all_state[socketfd];

  if (client_context->nb_write_left > 0)
  {
    int msg_sent = write(socketfd, poem, strlen(poem));

    if (msg_sent == -1)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        return client_write;
      else
        terminate("write_failed");
    }
    client_context->nb_write_left--;
  }
  else if (client_context->nb_write_left == 0)
  {
    return client_noevent;
  }
  return client_write;
}

int       main()
{
  int     event_count;
  struct  epoll_event event, events[MAX_CONNECTIONS];
  int     epfd = epoll_create1(0);

  printf("Serving on %d\n", PORT);
  int     server_fd = initiate_socket();

  if (epfd == -1)
  {
    fprintf(stderr, "Failed to create epoll fd\n");
    exit(EXIT_FAILURE);
  }

  event.data.fd = server_fd;
  event.events = EPOLLIN;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &event) == -1)
  {
    fprintf(stderr, "Failed to add fd to epoll\n");
    close_epfd(epfd);
  }

  while(1)
  {
    event_count = epoll_wait(epfd, events, MAX_CONNECTIONS, -1);
    for (int i = 0; i < event_count; i++)
    {
      if (events[i].data.fd == server_fd)
      {
        int client_fd = accept_connection(server_fd);
        if (client_fd == -1)
        {
          terminate("Couldn't connect client");
        }
        non_blocking_socket(client_fd);

        if (client_fd >= MAX_CONNECTIONS)
        {
          terminate("Maximum of connection reached");
        }
        client_status_t status = initate_context(client_fd);
        struct epoll_event event = { .data.fd = client_fd };

        if (status.want_read)
        {
          event.events |= EPOLLIN;
        }
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event) == -1)
        {
          terminate("Epoll add");
        }
      }
      else
      {
        int fd = events[i].data.fd;
        struct epoll_event event = { .data.fd = fd };
        client_status_t status;

        if (events[i].events & EPOLLIN)
        {
          printf("reading on %d...\n", fd);
          status = read_request(fd);

          if (status.want_read)
          {
            event.events |= EPOLLIN;
          }
          if (status.want_write)
          {
            event.events |= EPOLLOUT;
          }
          if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event) == -1)
          {
            terminate("Epoll mod");
          }
        }
        else if (events[i].events & EPOLLOUT)
        {
          printf("writing on %d...\n", fd);
          status = write_request(fd);
          if (status.want_write)
          {
            event.events |= EPOLLOUT;
          }
          if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event) == -1)
          {
            terminate("Epoll mod");
          }
        }
        if (event.events == 0)
        {
          printf("Closing socket %d...\n", fd);
          if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
          {
            terminate("Epoll del");
          }
        }
      }
    }
  }

  close_epfd(epfd);
  return 0;
}