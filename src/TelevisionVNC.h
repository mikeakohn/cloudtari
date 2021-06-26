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

#ifndef TELEVISION_VNC_H
#define TELEVISION_VNC_H

#include <stdint.h>

#include "Television.h"

class TelevisionVNC : public Television
{
public:
  TelevisionVNC();
  virtual ~TelevisionVNC();

  virtual int init();
  virtual void clear_display();
  virtual void draw_pixel(int x, int y, uint32_t color);
  virtual bool refresh();
  virtual int handle_events();
  virtual void set_port(int value) { port = value; };

private:
  int vnc_send(const uint8_t *buffer, int len);
  int vnc_recv(uint8_t *buffer, int len);
  bool vnc_has_data();
  void close_connection();
  int send_protocol_version();
  int get_client_protocol_version();
  int send_security();
  int get_client_init();
  int send_server_init();
  int send_image_full();
  int send_image_update(int x, int y, int width, int height);

  int socket_id;
  int client;
  int port;

  struct ImagePacket
  {
    uint8_t header[16];
    uint32_t data[];
  } *image_packet;
};

#endif

