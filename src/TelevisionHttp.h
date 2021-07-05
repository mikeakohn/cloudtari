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

#include "GifCompressor.h"
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
  int send_gif();

  uint8_t *image;
  GifCompressor *gif_compressor;

  struct timeval refresh_time;


};

#endif

