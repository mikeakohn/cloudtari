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

#ifndef TELEVISION_NULL_H
#define TELEVISION_NULL_H

#include <stdint.h>

#include "Television.h"

class TelevisionNull : public Television
{
public:
  TelevisionNull();
  virtual ~TelevisionNull();

  virtual int init();
  virtual void clear_display();
  virtual void draw_pixel(int x, int y, uint32_t color);
  virtual bool refresh();
  virtual int handle_events();

private:

};

#endif

