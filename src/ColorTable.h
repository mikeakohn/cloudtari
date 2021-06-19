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

#ifndef COLOR_TABLE_H
#define COLOR_TABLE_H

#include <stdint.h>

class ColorTable
{
public:
  static uint32_t get_color(int index);

private:
  ColorTable() { }
  ~ColorTable() { }

  static uint32_t colors[128];
};

#endif

