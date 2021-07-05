/**
 *  Cloudtari
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2021 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Network.h"

Network::Network() :
  socket_id{-1},
  client{-1},
  port{5900}
{
}

Network::~Network()
{
  net_close();
}

int Network::net_open(int port)
{
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;

  socket_id = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_id < 0)
  {
    printf("Can't open socket.\n");
    return -1;
  }

  memset((char*)&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  if (bind(socket_id, (const sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("Server can't bind.\n");
    return -1;
  }

  if (listen(socket_id, 1) != 0)
  {
    printf("Listen failed.\n");
    return -1;
  }

  socklen_t n = sizeof(client_addr);

  client = accept(socket_id, (struct sockaddr *)&client_addr, &n);

  if (client == -1) { return -1; }

  fcntl(client, F_SETFL, O_NONBLOCK);

  return 0;
}

void Network::net_close()
{
  if (socket_id == -1) { return; }
  close(socket_id);
  socket_id = -1;
}

int Network::net_send(const uint8_t *buffer, int length)
{
  struct timeval tv;
  fd_set writeset;

  int bytes_sent = 0;

  while (bytes_sent < length)
  {
    FD_ZERO(&writeset);
    FD_SET(client, &writeset);

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    int n = select(client + 1, NULL, &writeset, NULL, &tv);

    if (n == -1)
    {
      if (errno == EINTR) { continue; }

      perror("Problem with select in net_send");
      return -1;
    }

    if (n == 0) { return -3; }

    n = send(client, buffer + bytes_sent, length - bytes_sent, 0);
    if (n < 0) { return -4; }

    bytes_sent += n;
  }

  return bytes_sent;
}

int Network::net_recv(uint8_t *buffer, int length, bool wait_for_full_buffer)
{
  struct timeval tv;
  fd_set readset;

  int bytes_received = 0;

  while (bytes_received < length)
  {
    FD_ZERO(&readset);
    FD_SET(client, &readset);

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    int n = select(client + 1, &readset, NULL, NULL, &tv);

    if (n == -1)
    {
      if (errno == EINTR) { continue; }

      perror("Problem with select in net_recv");
      return -1;
    }

    if (n == 0) { return -3; }

    n = recv(client, buffer + bytes_received, length - bytes_received, 0);
    if (n < 0) { return -4; }

    bytes_received += n;

    if (!wait_for_full_buffer) { break; }
  }

  return bytes_received;
}

