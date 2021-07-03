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
  uint8_t read(int address);
  void write(int address, uint8_t value);
  void dump(int start, int end);
  void clock(int cycles);
  PIA *get_pia() { return pia; }
  TIA *get_tia() { return tia; }

  uint16_t read16(int address)
  {
    return read(address) | (read(address + 1) << 8);
  }

private:
  ROM *rom;
  PIA *pia;
  TIA *tia;
};

#endif

