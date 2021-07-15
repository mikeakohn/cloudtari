/**
 *  Cloudtari
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2021 by Michael Kohn
 *
 * TelevisionVNC can display the Atari 2600 video over a network using
 * the VNC remote desktop protocol. Keyboard commands are transmitted
 * back to the class also.
 *
 */

#ifndef TELEVISION_VNC_H
#define TELEVISION_VNC_H

#include <stdint.h>

#include "Network.h"
#include "Television.h"

class TelevisionVNC : public Television, public Network
{
public:
  TelevisionVNC();
  virtual ~TelevisionVNC();

  virtual int init();
  //virtual void clear_display();
  //virtual void draw_pixel(int x, int y, uint32_t color);
  //virtual void draw_pixel(int x, int y, uint8_t color);
  virtual bool refresh();
  virtual int handle_events();
  virtual void *get_image() { return image_packet[image_page]->data; }
  virtual int get_bitsize() { return 32; }
  virtual void set_port(int value) { port = value; };

private:
  inline void set_pixel(int x, int y, uint32_t color);
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

  bool needs_full_image;
  bool needs_color_table;
  int image_packet_length;

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

