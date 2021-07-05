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

#ifndef TELEVISION_HTTP_H
#define TELEVISION_HTTP_H

#include <stdint.h>

#include "Network.h"
#include "Television.h"

class TelevisionHttp : public Television, public Network
{
public:
  TelevisionHttp();
  virtual ~TelevisionHttp();

  virtual int init();
  virtual void clear_display();
  virtual void draw_pixel(int x, int y, uint32_t color);
  virtual void draw_pixel(int x, int y, uint8_t color);
  virtual bool refresh();
  virtual int handle_events();
  virtual void set_port(int value) { port = value; };

private:
  int read_http();
  int send_index_html();

#if 0
  int net_open(int port);
  void net_close();
  int net_send(const uint8_t *buffer, int len);
  int net_recv(uint8_t *buffer, int len);


  int socket_id;
  int client;
  int port;
#endif
  uint8_t *image;

  struct timeval refresh_time;


};

#endif

