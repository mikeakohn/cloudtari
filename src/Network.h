/**
 *  Cloudtari
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2021 by Michael Kohn
 *
 * Network is used to abstract out all the socket() functionality and
 * is currently used by TelevisionHttp and TelevisionVNC.
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
  bool net_has_data();
  bool net_is_connected() { return socket_id != -1; }

  int socket_id;
  int client;
  int port;
};

#endif

