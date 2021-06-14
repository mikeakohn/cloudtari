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

#ifndef TIA_H
#define TIA_H

#include <stdint.h>

class TIA
{
public:
  TIA();
  ~TIA();

  void init();
  uint8_t read_memory(int address);
  void write_memory(int address, uint8_t value);

private:
};

#endif

