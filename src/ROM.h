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

#ifndef ROM_H
#define ROM_H

#include <stdint.h>

class ROM
{
public:
  ROM();
  ~ROM();

  int load(const char *filename);

  uint8_t read_int8(int address)
  {
    return memory[address];
  }

  uint8_t read_int16(int address)
  {
    return memory[address] | memory[address + 1] << 8;
  }

private:
  uint8_t memory[4096];
};

#endif

