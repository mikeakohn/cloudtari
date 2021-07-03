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

#ifndef TELEVISION_VNC_H
#define TELEVISION_VNC_H

#include <stdint.h>
#include <sys/time.h>

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
  int send_color_table();
  int send_image_full();
  int send_image_diff();
  int send_image_update(int x, int y, int width, int height, bool incremental);
  void print_pixel_format(uint8_t *buffer);
  void print_encoding(uint8_t *buffer);

  int socket_id;
  int client;
  int port;

  bool needs_full_image;
  bool needs_color_table;
  int image_packet_length;
  struct timeval refresh_time;

  struct ImagePacket
  {
    uint8_t message_type; 
    uint8_t padding; 
    uint16_t number_of_rectangles; 
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    int encoding_type;
    uint32_t data[];
  } *image_packet[2];

  int image_page;

  struct FramebufferUpdate
  {
    uint8_t message_type;
    uint8_t padding;
    uint16_t number_of_rectangles;
  };

  struct UpdateRectangle
  {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    int encoding_type;
  };

#if 0
  struct ScanlineChange
  {
    uint16_t left;
    uint16_t right;
  } *scanline_change;
#endif

  uint8_t *diff_buffer;
  int diff_buffer_length;

  enum
  {
    ENCODING_RAW = 0,
    ENCODING_COPY_RECT = 1,
    ENCODING_RRE = 2,
    ENCODING_CORRE = 4,
    ENCODING_HEXTILE = 5,
    ENCODING_ZLIB = 6,
    ENCODING_TRLE = 15,
    ENCODING_ZRLE = 16,
  };
};

#endif

