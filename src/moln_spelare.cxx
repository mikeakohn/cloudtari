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
#include "TelevisionVNC.h"
#include "TIA.h"

int main(int argc, char *argv[])
{
  int cycles;
  bool debug = false;
  bool step = false;
  int step_address = -1;
  int port = 5900;
  Television *television;

  // Used to see how many CPU cycles a set of instructions takes.
  DebugTimer debug_timer;

  if (argc < 3 || argc > 5)
  {
    printf(
      "Usage: %s <gamefile.bin> <null/sdl/vnc/debug/break/timer/step>\n"
      "          null\n"
      "          sdl\n"
      "          vnc <port>\n"
      "          debug\n"
      "          break <address>\n"
      "          timer <start_address> <end_address>\n"
      "          step <start_address>\n",
      argv[0]);
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
  if (strcmp(argv[2], "vnc") == 0)
  {
    television = new TelevisionVNC();

    if (argc > 3) { port = atoi(argv[3]); }

    television->set_port(port);
  }
    else
  if (strcmp(argv[2], "debug") == 0)
  {
    television = new TelevisionNull();
    debug = true;
    m6502->set_debug();
  }
    else
  if (strcmp(argv[2], "break") == 0)
  {
    television = new TelevisionVNC();

    int address = 0xf000;
    if (argc > 3) { address = atoi(argv[3]); }
    m6502->set_breakpoint(address);
  }
    else
  if (strcmp(argv[2], "timer") == 0)
  {
    television = new TelevisionNull();
    debug = true;
    m6502->set_debug();

    int address_start, address_end;

    if (argc > 3) { address_start = strtol(argv[3], NULL, 0); }
    if (argc > 4) { address_end = strtol(argv[4], NULL, 0); }

    debug_timer.set(address_start, address_end);
  }
    else
  if (strcmp(argv[2], "step") == 0)
  {
    television = new TelevisionNull();
    debug = true;
    m6502->set_debug();

    step_address = 0xf000;
    if (argc > 3) { step_address = strtol(argv[3], NULL, 0); }
  }
    else
  {
    television = new TelevisionNull();
  }

  if (television->init() != 0)
  {
    delete m6502;
    delete rom;
    delete memory_bus;
    delete television;

    printf("Display init error.\n");
    return -1;
  }

  PIA *pia = memory_bus->get_pia();
  TIA *tia = memory_bus->get_tia();
  tia->set_television(television);

  // memory_bus->dump(0xf000, 0xffff);

  while (m6502->is_running())
  {
    if (m6502->get_pc() == step_address)
    {
      step = true;
    }

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
        break;
      case 2:
        pia->clear_switch_reset();
        break;
    }

    if (step)
    {
      getchar();
    }
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

