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
  virtual bool refresh() = 0;
  virtual int handle_events() = 0;
  virtual void set_port(int value) { };

private:

};

#endif

