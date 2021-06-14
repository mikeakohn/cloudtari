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

#ifndef MEMORY_BUS_H
#define MEMORY_BUS_H

#include "PIA.h"
#include "ROM.h"
#include "TIA.h"

class MemoryBus
{
public:
  MemoryBus();
  ~MemoryBus();

  void set_rom(ROM *rom) { this->rom = rom; }
  void init();
  uint8_t read_memory(int address);
  void write_memory(int address, uint8_t value);

private:
  ROM *rom;
  PIA *pia;
  TIA *tia;
};

#endif

