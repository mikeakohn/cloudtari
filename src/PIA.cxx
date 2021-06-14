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
  return 0;
}

void PIA::write_memory(int address, uint8_t value)
{
}

