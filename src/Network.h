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

#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>

class Network
{
protected:
  Network();
  ~Network();

  int net_open(int port);
  void net_close();
  int net_send(const uint8_t *buffer, int len);
  int net_recv(uint8_t *buffer, int len, bool wait_for_full_buffer = true);

  int socket_id;
  int client;
  int port;

};

#endif

