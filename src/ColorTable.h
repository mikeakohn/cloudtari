/**
 *  Cloudtari
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2021 by Michael Kohn
 *
 * This object basically represents the color palette of the Atari 2600.
 * it's used to convert a 7 bit index to a 32 bit color.
 *
 */

#ifndef COLOR_TABLE_H
#define COLOR_TABLE_H

#include <stdint.h>

class ColorTable
{
public:
  static uint32_t get_color(int index);
  static uint32_t *get_table() { return colors; }

private:
  ColorTable() { }
  ~ColorTable() { }

  static uint32_t colors[128];
};

#endif

