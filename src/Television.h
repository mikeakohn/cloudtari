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

#ifndef TELEVISION_H
#define TELEVISION_H

#include <stdint.h>

class Television
{
public:
  Television();
  virtual ~Television();

  virtual int init() = 0;
  virtual void clear_display() = 0;
  virtual void draw_pixel(int x, int y, uint32_t color) = 0;
  virtual void draw_pixel(int x, int y, uint8_t color) = 0;
  virtual bool refresh() = 0;
  virtual int handle_events() = 0;
  virtual void set_port(int value) { };

  enum
  {
    KEY_QUIT = 1,
    KEY_SELECT_DOWN,
    KEY_SELECT_UP,
    KEY_RESET_DOWN,
    KEY_RESET_UP,
    KEY_LEFT_DOWN,
    KEY_LEFT_UP,
    KEY_RIGHT_DOWN,
    KEY_RIGHT_UP,
    KEY_UP_DOWN,
    KEY_UP_UP,
    KEY_DOWN_DOWN,
    KEY_DOWN_UP,
    KEY_FIRE_DOWN,
    KEY_FIRE_UP,
  };

protected:
  int width, height;

private:

};

#endif

