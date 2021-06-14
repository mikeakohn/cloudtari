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

#ifndef M6502_H
#define M6502_H

#include <stdint.h>

#include "MemoryBus.h"

class M6502
{
public:
  M6502();
  ~M6502();

  int execute_instruction();

  void reset();
  void dump();
  void illegal_instruction();

private:
  int reg_a, reg_x, reg_y;
  uint16_t pc, sp;
  uint32_t total_cycles;
  uint32_t total_instructions;

  union Status
  {
    Status() { reset(); }

    void reset()
    {
      value = 0;
      one = 1;
      z = 1;
    }

    uint8_t value;

    // FIXME: This is possibly going to have issues on big endian systems.
    struct
    {
      uint8_t c : 1;
      uint8_t z : 1;
      uint8_t i : 1;
      uint8_t d : 1;
      uint8_t b : 1;
      uint8_t one : 1;
      uint8_t v : 1;
      uint8_t n : 1;
    }; 
  } reg_p;
};

#endif

