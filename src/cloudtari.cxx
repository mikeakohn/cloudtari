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
#include <unistd.h>

#include "DebugTimer.h"
#include "M6502.h"
#include "MemoryBus.h"
#include "ROM.h"
#include "TelevisionHttp.h"
#include "TelevisionNull.h"
#ifdef USE_SDL
#include "TelevisionSDL.h"
#endif
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
#ifdef USE_SDL
      "          sdl\n"
#endif
      "          vnc <port>\n"
      "          http <port>\n"
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

  memory_bus->init();
  memory_bus->set_rom(rom);
  m6502->set_memory_bus(memory_bus);
  m6502->reset();

#ifdef USE_SDL
  if (strcmp(argv[2], "sdl") == 0)
  {
    television = new TelevisionSDL();
  }
    else
#endif
  if (strcmp(argv[2], "vnc") == 0)
  {
    television = new TelevisionVNC();

    if (argc > 3) { port = atoi(argv[3]); }

    television->set_port(port);
  }
    else
  if (strcmp(argv[2], "http") == 0)
  {
    television = new TelevisionHttp();

    port = 8080;
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
    television = new TelevisionNull();

    int address = 0xf000;
    if (argc > 3) { address = strtol(argv[3], NULL, 0); }
    debug = true;
    m6502->set_breakpoint(address);
  }
    else
  if (strcmp(argv[2], "timer") == 0)
  {
    television = new TelevisionNull();
    debug = true;
    m6502->set_debug();

    int address_start = -1, address_end = -1;

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
  if (strcmp(argv[2], "null") == 0)
  {
    television = new TelevisionNull();
  }
    else
  {
    printf("Unknown mode %s\n", argv[2]);
    exit(1);
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

  RIOT *riot = memory_bus->get_riot();
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

    if (tia->need_check_events())
    {
      int event_code = television->handle_events();

      if (event_code == Television::KEY_QUIT) { break; }

      switch (event_code)
      {
        case Television::KEY_SELECT_DOWN:
          riot->set_switch_select();
          break;
        case Television::KEY_SELECT_UP:
          riot->clear_switch_select();
          break;
        case Television::KEY_RESET_DOWN:
          riot->set_switch_reset();
          break;
        case Television::KEY_RESET_UP:
          riot->clear_switch_reset();
          break;
        case Television::KEY_LEFT_DOWN:
          riot->set_joystick_0_left();
          break;
        case Television::KEY_LEFT_UP:
          riot->clear_joystick_0_left();
          break;
        case Television::KEY_RIGHT_DOWN:
          riot->set_joystick_0_right();
          break;
        case Television::KEY_RIGHT_UP:
          riot->clear_joystick_0_right();
          break;
        case Television::KEY_UP_DOWN:
          riot->set_joystick_0_up();
          break;
        case Television::KEY_UP_UP:
          riot->clear_joystick_0_up();
          break;
        case Television::KEY_DOWN_DOWN:
          riot->set_joystick_0_down();
          break;
        case Television::KEY_DOWN_UP:
          riot->clear_joystick_0_down();
          break;
        case Television::KEY_FIRE_DOWN:
          tia->set_joystick_0_fire();
          break;
        case Television::KEY_FIRE_UP:
          tia->clear_joystick_0_fire();
          break;
      }
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

