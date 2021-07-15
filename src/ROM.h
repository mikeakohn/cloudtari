/**
 *  Cloudtari
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2021 by Michael Kohn
 *
 * ROM is used to load a catridge ROM file and deal with bank switching
 * and such. MemoryBus accesses this object if the CPU requests a read
 * from a memory location with the ROM as a destination address.
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
  bool set_bank(int value);

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
  uint8_t full[8192];
  int size;
};

#endif

