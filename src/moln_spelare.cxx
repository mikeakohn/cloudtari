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

#include "M6502.h"
#include "MemoryBus.h"
#include "ROM.h"

int main(int argc, char *argv[])
{
  M6502 *m6502 = new M6502();
  MemoryBus *memory_bus = new MemoryBus();
  ROM *rom = new ROM();

  if (argc != 2)
  {
    printf("Usage: %s <gamefile.bin>\n", argv[0]);
    exit(0);
  }

  if (rom->load(argv[0]) != 0) { exit(-1); }

  memory_bus->set_rom(rom);
  m6502->set_memory_bus(memory_bus);

  delete m6502;
  delete rom;
  delete memory_bus;

  return 0;
}
