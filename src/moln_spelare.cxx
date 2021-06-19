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
#include <unistd.h>

#include "M6502.h"
#include "MemoryBus.h"
#include "ROM.h"
#include "TelevisionNull.h"
#include "TelevisionSDL.h"
#include "TIA.h"

int main(int argc, char *argv[])
{
  int cycles;
  bool debug = false;
  Television *television;

  if (argc != 3)
  {
    printf("Usage: %s <gamefile.bin> <sdl/null>\n", argv[0]);
    exit(0);
  }

  M6502 *m6502 = new M6502();
  MemoryBus *memory_bus = new MemoryBus();
  ROM *rom = new ROM();

  if (rom->load(argv[1]) != 0) { exit(-1); }

  memory_bus->set_rom(rom);
  m6502->set_memory_bus(memory_bus);
  m6502->reset();

  if (strcmp(argv[2], "sdl") == 0)
  {
    television = new TelevisionSDL();
  }
    else
  if (strcmp(argv[2], "debug") == 0)
  {
    television = new TelevisionNull();
    debug = true;
    m6502->set_debug();
  }
    else
  {
    television = new TelevisionNull();
  }

  if (debug) { m6502->set_debug(); }
  //m6502->set_breakpoint(0xf45e);

  television->init();

  TIA *tia = memory_bus->get_tia();
  tia->set_television(television);

  // memory_bus->dump(0xf000, 0xffff);

  while (m6502->is_running())
  {
    if (tia->wait_for_hsync())
    {
      cycles = 1;
      m6502->clock(cycles);
    }
      else
    {
      cycles = m6502->step();
    }

    memory_bus->clock(cycles);

    if (debug)
    {
      printf("  cycles=%d\n", cycles);
      m6502->dump();
      tia->dump();
      memory_bus->dump(0x80, 0xff);

      if (!tia->wait_for_hsync())
      {
        //sleep(1);
        usleep(1000);
      }
    }

    if (television->handle_events() == -1) { break; }

    //sleep(1);

    //television->clear_display();
  }

  delete m6502;
  delete rom;
  delete memory_bus;
  delete television;

  return 0;
}

