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

#include "MemoryBus.h"
#include "RIOT.h"
#include "TIA.h"

MemoryBus::MemoryBus() : rom{nullptr}
{
  tia = new TIA();
  riot = new RIOT();
}

MemoryBus::~MemoryBus()
{
  delete tia;
  delete riot;
}

void MemoryBus::init()
{
  tia->reset();
}

uint8_t MemoryBus::read(int address)
{
  // Takes care of mirrored memory.
  if ((address & 0x1000) == 0x1000)
  {
    if (address == 0x1ff9)
    {
      if (rom->set_bank(1)) { return 0; }
    }
      else
    if (address == 0x1ff8)
    {
      if (rom->set_bank(0)) { return 0; }
    }

    return rom->read_int8(address & 0x0fff);
  }

  if (address <= 0x3f)
  {
    return tia->read_memory(address & 0x3f);
  }

  return riot->read_memory(address);
}

void MemoryBus::write(int address, uint8_t value)
{
  if ((address & 0x1000) == 0x1000)
  {
  }
    else
  if (address <= 0x3f)
  { 
    tia->write_memory(address & 0x3f, value);
  } 
    else
  if ((address & 0x1000) == 0x0000)
  { 
    riot->write_memory(address, value);
  }
}

void MemoryBus::dump(int start, int end)
{
  start &= 0xfff0;

  for (int n = start; n <= end; n++)
  {
    if ((n % 16) == 0)
    {
      printf("%04x:", n);
    }

    if ((n % 8) == 0)
    {
      printf(" " );
    }

    printf(" %02x", read(n));

    if ((n % 16) == 15)
    {
      printf("\n");
    }
  }

  printf("\n");
}

void MemoryBus::clock(int cycles)
{
  tia->clock(cycles);
  riot->clock(cycles);
}

