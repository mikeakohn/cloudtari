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

#include "ROM.h"

class MemoryBus
{
public:
  MemoryBus();
  ~MemoryBus();

  void set_rom(ROM *rom) { this->rom = rom; }

private:
  ROM *rom;
};

#endif

