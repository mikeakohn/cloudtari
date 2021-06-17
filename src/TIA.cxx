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

#include "TIA.h"

TIA::TIA() :
  pos_x(0),
  wsync(0)
{
}

TIA::~TIA()
{
}

void TIA::init()
{
#if 0
  television.init();
#endif
  pos_x = 0;
  wsync = 0;
  //pf_bit = 1;
}

uint8_t TIA::read_memory(int address)
{
  return 0;
}

void TIA::write_memory(int address, uint8_t value)
{
}

void TIA::clock()
{ 
  if (pos_x >= 68)
  {
    calculate_pixel();
  }
    else
  {
    pos_x++;
  }

  //television.clock();
}

void TIA::clock(int ticks)
{
  ticks = ticks * 3;

  while (ticks > 0 || wsync == 1)
  {
    clock();
    ticks--;
  }
}

void TIA::calculate_pixel()
{
  // The main playfield area starts after 68 clocks. The pf_bit will point
  // to the next bit to be displayed from PF0, PF1, PF2.
  if (pos_x == 68) { pf_bit = 1; }
}

