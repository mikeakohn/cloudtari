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
#include <stdint.h>
#include <string.h>

#include "PIA.h"

PIA::PIA()
{
  reset();
}

PIA::~PIA()
{
}

void PIA::reset()
{
  memset(ram, 0, sizeof(ram));
  memset(riot, 0, sizeof(riot));

  riot[0] = 255;

  prescale = TIM1T;
  prescale_shift = TIM1T_SHIFT;
}

uint8_t PIA::read_memory(int address)
{
  if (address >= 128 && address <= 255)
  {
    return ram[address];
  }

#if 0
  if (address == INTIM)
  {
    return interrupt_timer;
  }
#endif

  return riot[address & 0x07];
}

void PIA::write_memory(int address, uint8_t value)
{
  // 128 bytes of RAM.
  if (address >= 128 && address <= 255)
  {
    ram[address] = value;
    return;
  }

  if (address >= 0x294)
  {
    // The timer is set by writing a value or count (from 1 to 255).
    // to the address of the desired interval setting according to
    // the following table:
    switch (address)
    {
      case 0x294:
        // 0x294: 1 clock
        prescale = TIM1T;
        prescale_shift = TIM1T_SHIFT;
        break;
      case 0x295:
        // 0x295: 8 clocks
        prescale = TIM8T;
        prescale_shift = TIM8T_SHIFT;
        break;
      case 0x296:
        // 0x296: 64 clocks
        prescale = TIM64T;
        prescale_shift = TIM64T_SHIFT;
        break;
      case 0x297:
        // 0x297: 1024 clocks
        prescale = T1024T;
        prescale_shift = T1024T_SHIFT;
        break;
    }

    interrupt_timer = ((value + 1) * prescale) - 1;
    riot[4] = value;

    return;
  }

  if (address < 0x284)
  {
    riot[address & 0x07] = value;
  }
}

void PIA::clock(int ticks)
{
  interrupt_timer -= ticks;

  while (interrupt_timer < 0)
  {
    interrupt_timer += prescale << 8;
  }

  riot[4] = interrupt_timer >> prescale_shift;
}

