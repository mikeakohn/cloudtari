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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "TelevisionNull.h"

TelevisionNull::TelevisionNull()
{
  image = (uint32_t *)malloc(width * height * sizeof(uint32_t));
}

TelevisionNull::~TelevisionNull()
{
  free(image);
}

int TelevisionNull::init()
{
  return 0;
}

#if 0
void TelevisionNull::clear_display()
{
}

void TelevisionNull::draw_pixel(int x, int y, uint32_t color)
{
}

void TelevisionNull::draw_pixel(int x, int y, uint8_t color)
{
}
#endif

bool TelevisionNull::refresh()
{
  return true;
}

int TelevisionNull::handle_events()
{
  return 0;
}

