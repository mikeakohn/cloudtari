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

  void set_memory_bus(MemoryBus *memory_bus) { this->memory_bus = memory_bus; }
  void set_debug() { debug = true; }
  void stop() { }
  void reset();
  void dump();
  void illegal_instruction(uint8_t opcode);
  int execute_instruction();

private:
  int read_immediate()
  {
    return memory_bus->read(pc++);
  }

  int read_absolute()
  {
    int m = memory_bus->read16(pc);
    pc += 2;
    return memory_bus->read(m);
  }

  int read_absolute(int &address)
  {
    int m = memory_bus->read16(pc);
    address = m;
    pc += 2;
    return memory_bus->read(m);
  }

  int read_zero_page()
  {
    int m = memory_bus->read(pc++);
    return memory_bus->read(m);
  }

  int read_indirect_x()
  {
    int m;
    m = memory_bus->read(pc++) + reg_x;
    m = memory_bus->read16(m & 0xff);
    return memory_bus->read(m);
  }

  void set_flags(int &data)
  {
    status.c = (data & 0x100) != 0;
    data &= 0xff;

    status.z = data == 0;
    status.n = (data & 0x80) != 0;
  }

  void push(uint8_t data)
  {
    memory_bus->write(sp--, data);
  }

  MemoryBus *memory_bus;

  int reg_a, reg_x, reg_y;
  uint16_t pc, sp;
  uint32_t total_cycles;
  uint32_t total_instructions;
  bool debug;

  union Status
  {
    Status() { reset(); }

    void reset()
    {
      reg_p = 0;
      one = 1;
      z = 1;
    }

    uint8_t reg_p;

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
  } status;
};

#endif

