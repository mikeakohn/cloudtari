/**
 *  Cloudtari
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2021 by Michael Kohn
 *
 * TelevisionHttp implements a small web server that allows only one
 * connection. When a request for / comes in the a web page with some
 * simple Javascript will start requesting GIF images from this server
 * 30 times a second. Keyboard presses are captured and put in a
 * "queue" variable that are transmitted back to the server when new
 * GIF images are requested.
 *
 */

#ifndef TELEVISION_HTTP_H
#define TELEVISION_HTTP_H

#include <stdint.h>
#include <string.h>

#include "GifCompressor.h"
#include "Network.h"
#include "Television.h"

class TelevisionHttp : public Television, public Network
{
public:
  TelevisionHttp();
  virtual ~TelevisionHttp();

  virtual int init();
  //virtual void clear_display();
  //virtual void draw_pixel(int x, int y, uint32_t color);
  //virtual void draw_pixel(int x, int y, uint8_t color);
  virtual bool refresh();
  virtual int handle_events();
  virtual void *get_image() { return image; }
  virtual int get_bitsize() { return 8; }
  virtual void set_port(int value) { port = value; };

private:
  int read_http();
  int send_index_html();
  int send_gif();
  int send_404();

  uint8_t *image;
  uint8_t *gif;
  int gif_length;
  int no_data_count;
  GifCompressor *gif_compressor;
  char query_string[128];
  char filename[128];

  struct KeyQueue
  {
    KeyQueue() : count{0}, head{0}, tail{0}
    {
      memset(queue, 0, sizeof(queue));
    }

    bool is_empty() { return count == 0; }
    bool is_full() { return count == sizeof(queue); }

    char get_next()
    {
      char c = queue[head++];
      if (head == sizeof(queue)) { head = 0; }
      count--;
      return c;
    }

    void append(char c)
    {
      if (is_full()) { return; }
      queue[tail++] = c;
      if (tail == sizeof(queue)) { tail = 0; }
      count++;
    }

    int count, head, tail;
    char queue[32];
  } key_queue;
};

#endif

