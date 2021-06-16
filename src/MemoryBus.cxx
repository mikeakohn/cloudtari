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

#include <stdio.h>
#include <stdlib.h>

#include "MemoryBus.h"
#include "PIA.h"
#include "TIA.h"

MemoryBus::MemoryBus() : rom(nullptr)
{
  tia = new TIA();
  pia = new PIA();
}

MemoryBus::~MemoryBus()
{
  delete tia;
  delete pia;
}

void MemoryBus::init()
{
  tia->init();
}

uint8_t MemoryBus::read(int address)
{
  // Takes care of crappy mirrored memory.
  if ((address & 0x1000) == 0x1000)
  {
    return rom->read_int8(address & 0x0fff);
  }
    else
  if (address <= 0x3f)
  {
    return tia->read_memory(address & 0x3f);
  }

  return pia->read_memory(address);
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
    pia->write_memory(address, value);
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

    printf(" %02x", read(n));

    if ((n % 16) == 15)
    {
      printf("\n");
    }
  }

  printf("\n");
}

