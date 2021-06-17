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
  prescale_s = TIM1T_S;
}

uint8_t PIA::read_memory(int address)
{
  if (address >= 128 && address <= 255)
  {
    return ram[address];
  }

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
    if (address == 0x294)
    {
      prescale = TIM1T;
      prescale_s = TIM1T_S;
    }
      else
    if (address == 0x295)
    {
      prescale = TIM8T;
      prescale_s = TIM8T_S;
    }
      else
    if (address == 0x296)
    {
      prescale = TIM64T;
      prescale_s = TIM64T_S;
    }
      else
    if (address == 0x297)
    {
      prescale = T1024T;
      prescale_s = T1024T_S;
    }

    interrupt_timer = ((value + 1) * prescale) - 1;
    riot[4] = value;
  }
    else
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

  riot[4] = interrupt_timer >> prescale_s;
}

