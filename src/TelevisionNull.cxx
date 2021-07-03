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

#include "TelevisionNull.h"

TelevisionNull::TelevisionNull()
{
}

TelevisionNull::~TelevisionNull()
{
}

int TelevisionNull::init()
{
  return 0;
}

void TelevisionNull::clear_display()
{
}

void TelevisionNull::draw_pixel(int x, int y, uint32_t color)
{
}

bool TelevisionNull::refresh()
{
  return true;
}

int TelevisionNull::handle_events()
{
  return 0;
}

