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

#include "TIA.h"

TIA::TIA()
{
}

TIA::~TIA()
{
}

void TIA::init()
{
#if 0
  television.init();
  pf_bit = 1;
  pos_x = 0;
#endif
}

uint8_t TIA::read_memory(int address)
{
  return 0;
}

void TIA::write_memory(int address, uint8_t value)
{
}

