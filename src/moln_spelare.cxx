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

#include "DebugTimer.h"
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
  //m6502->set_breakpoint(0xf060);
  //m6502->set_breakpoint(0xf13d);
  //m6502->set_breakpoint(0xf622);

  television->init();

  PIA *pia = memory_bus->get_pia();
  TIA *tia = memory_bus->get_tia();
  tia->set_television(television);

  // memory_bus->dump(0xf000, 0xffff);

#if 0
  DebugTimer debug_timer;
  debug_timer.set(0xf61b, 0xf643);
  //debug_timer.set(0xf645, 0xf64a);
#endif

  while (m6502->is_running())
  {
    if (tia->wait_for_hsync())
    {
      cycles = 1;
      m6502->clock();
    }
      else
    {
      //int address = m6502->get_pc();
      cycles = m6502->step();

      //debug_timer.compute(address, cycles);
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

    int event_code = television->handle_events();

    if (event_code == -1) { break; }

    switch (event_code)
    {
      case 1:
        pia->set_switch_reset();
printf("reset down\n");
        break;
      case 2:
printf("reset up\n");
        pia->clear_switch_reset();
        break;
    }

    //television->clear_display();
  }

#if 0
   m6502->dump();
   tia->dump();
   memory_bus->dump(0x80, 0xff);
#endif

  delete m6502;
  delete rom;
  delete memory_bus;
  delete television;

  return 0;
}

