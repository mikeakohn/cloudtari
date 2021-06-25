/**
 *  Moln Spelare
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

#include "TelevisionVNC.h"

TelevisionVNC::TelevisionVNC() :
  socket_id(-1),
  client(-1),
  port(5900)
{
}

TelevisionVNC::~TelevisionVNC()
{
}

int TelevisionVNC::init()
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
  }

  socklen_t n = sizeof(client_addr);

  client = accept(socket_id, (struct sockaddr *)&client_addr, &n);

  fcntl(client, F_SETFL, O_NONBLOCK);

  return 0;
}

void TelevisionVNC::clear_display()
{
}

void TelevisionVNC::draw_pixel(int x, int y, uint32_t color)
{
}

bool TelevisionVNC::refresh()
{
  return true;
}

int TelevisionVNC::handle_events()
{
  return 0;
}

int TelevisionVNC::vnc_send(uint8_t *buffer, int len)
{
  struct timeval tv;
  fd_set writeset;
  int t, p;

  FD_ZERO(&writeset);
  FD_SET(client, &writeset);

  tv.tv_sec = 10;
  tv.tv_usec = 0;

  t = select(client + 1, NULL, &writeset, NULL, &tv);

  if (t == -1 && errno != EINTR)
  {
    perror("Problem with select in vnc_send");
    return -1;
  }

    if (t==-1) return -2;
    if (t==0) return -3;

    p = 0;

    while (p < len)
    {
      t = send(client, buffer + p, len - p, 0);
      if (t < 0) { return -4; }

      p = p + t;
    }

  return p;
}

int TelevisionVNC::vnc_recv(uint8_t *buffer, int len)
{
  struct timeval tv;
  fd_set readset;
  int t, p;

  FD_ZERO(&readset);
  FD_SET(client,&readset);

  tv.tv_sec=10;
  tv.tv_usec=0;

  t = select(client + 1, &readset, NULL, NULL, &tv);

  if (t == -1 && errno != EINTR)
  {
    perror("Problem with select in vnc_recv");
    return -1;
  }

  if (t == -1) { return -2; }
  if (t == 0) { return -3; }

  p = 0;

  while (p < len)
  {
    t = recv(client, buffer + p, len - p, 0);
    if (t == -1) { continue; }
    if (t < 0) return -4;
    p = p + t;
  }

  return p;
}

int TelevisionVNC::send_protocol_version()
{
  return 0;
}

int TelevisionVNC::get_client_protocol_version()
{
  return 0;
}

