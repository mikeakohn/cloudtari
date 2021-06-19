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
  void stop() { running = false; }
  void reset();
  void dump();
  void illegal_instruction(uint8_t opcode);
  bool is_running() { return running; }
  void clock(int ticks) { total_cycles += ticks; }
  int step();

private:
  int execute_instruction();

  int read_immediate()
  {
    return memory_bus->read(pc++);
  }

  int read_address()
  {
    int m = memory_bus->read16(pc);
    pc += 2;
    return m;
  }

  int read_absolute(int &address)
  {
    int m = memory_bus->read16(pc);
    address = m;
    pc += 2;
    return memory_bus->read(m);
  }

  int read_zero_page(int &address)
  {
    int m = memory_bus->read(pc++);
    address = m;
    return memory_bus->read(m);
  }

  int read_absolute_x(int &address)
  {
    int m = memory_bus->read16(pc) + reg_x;
    address = m;
    pc += 2;
    return memory_bus->read(m);
  }

  int read_absolute_y(int &address)
  {
    int m = memory_bus->read16(pc) + reg_y;
    address = m;
    pc += 2;
    return memory_bus->read(m);
  }

  int read_indirect_x(int &address)
  {
    int m;
    m = memory_bus->read(pc++) + reg_x;
    m = memory_bus->read16(m & 0xff);
    address = m;
    return memory_bus->read(m);
  }

  int read_indirect_y(int &address)
  {
    int m;
    m = memory_bus->read(pc++);
    m = memory_bus->read16(m) + reg_y;
    m = m & 0xffff;
    address = m;
    return memory_bus->read(m);
  }

  int read_zero_page_x(int &address)
  {
    int m;
    m = memory_bus->read(pc++) + reg_x;
    m = m & 0xffff;
    address = m;
    return memory_bus->read(m);
  }

  int read_zero_page_y()
  {
    int m;
    m = memory_bus->read(pc++) + reg_y;
    m = m & 0xffff;
    return memory_bus->read(m);
  }

  void set_flags(int &data)
  {
    status.c = (data & 0x100) != 0;
    data &= 0xff;

    status.z = data == 0;
    status.n = (data & 0x80) != 0;
  }

  void set_load_flags(int &data)
  {
    status.z = data == 0;
    status.n = (data & 0x80) != 0;
  }

  void push(uint8_t data)
  {
    memory_bus->write(sp--, data);
  }

  uint8_t pop()
  {
    return memory_bus->read(++sp);
  }

  void run_adc(int data)
  {
    if (status.d == 0)
    {
      reg_a = reg_a + data + status.c;

      status.c = reg_a > 0xff;
      reg_a &= 0xff;
      status.z = reg_a == 0;
      status.n = (reg_a & 0x80) != 0;
      status.v = status.c ^ status.n;
    }
      else
    {
      int d0 = data & 0xf;
      int d1 = data >> 4;
      int a0 = reg_a & 0xf;
      int a1 = reg_a >> 4;

      a0 = a0 + d0 + status.c;

      if (a0 > 10)
      {
        a1 += a0 / 10;
        a0 = a0 % 10;
      }

      a1 = a1 + d1;

      status.c = a1 / 10 > 0;
      a1 = a1 % 10;

      reg_a = ((a1 << 4) | a0) & 0xff;
      status.z = reg_a == 0;
    }
  }

  void run_sbc(int data)
  {
    if (status.d == 0)
    {
      reg_a = reg_a - data - (status.c ^ 1);

      status.c = reg_a > 0xff;
      reg_a &= 0xff;
      status.z = reg_a == 0;
      status.n = (reg_a & 0x80) != 0;
      status.v = status.c ^ status.n;
    }
      else
    {
      int d0 = data & 0xf;
      int d1 = data >> 4;
      int a0 = reg_a & 0xf;
      int a1 = reg_a >> 4;

      a0 = a0 - d0 - (status.c ^ 1);

      if (a0 < 0)
      {
        a1--;
        a0 += 1;
      }

      a1 = a1 - d1;

      status.c = a1 / 10 > 0;
      a1 = a1 % 10;

      reg_a = ((a1 << 4) | a0) & 0xff;
      status.z = reg_a == 0;
    }
  }

  void run_compare(int reg, int data)
  {
    reg = reg - data;

    status.c = (reg & 0xff00) != 0;
    status.z = reg == 0;
    status.n = (reg & 0x80) != 0;
  }

  void run_and(int data)
  {
    reg_a = reg_a & data;
    set_flags(reg_a);
  }

  void run_or(int data)
  {
    reg_a = reg_a ^ data;
    set_flags(reg_a);
  }

  void run_eor(int data)
  {
    reg_a = reg_a ^ data;
    set_flags(reg_a);
  }

  void run_bit(int data)
  {
    data = data & reg_a;
    status.z = data == 0;
    status.n = (data & 0x80) != 0;
    status.v = (data & 0x40) != 0;
  }

  void run_ror_memory(int address, int data)
  {
    status.c = data & 1;
    data = data >> 1;
    data |= status.c << 7;
    status.z = data == 0;
    status.n = (data & 0x80) != 0;
    memory_bus->write(address, data);
  }

  void run_ror()
  {
    status.c = reg_a & 1;
    reg_a = reg_a >> 1;
    reg_a |= status.c << 7;
    status.z = reg_a == 0;
    status.n = (reg_a & 0x80) != 0;
  }

  void run_rol_memory(int address, int data)
  {
    data = data << 1;
    set_flags(data);
    data |= status.c;
    memory_bus->write(address, data);
  }

  void run_asl_memory(int address, int data)
  {
    data = data << 1;
    set_flags(data);
    memory_bus->write(address, data);
  }

  void run_lsr_memory(int address, int data)
  {
    data = data >> 1;
    set_flags(data);
    memory_bus->write(address, data);
  }

  void run_inc_memory(int address, int data)
  {
    data++;
    set_flags(data);
    memory_bus->write(address, data);
  }

  void run_dec_memory(int address, int data)
  {
    data--;
    set_flags(data);
    memory_bus->write(address, data);
  }

  int get_branch_cycles(int address)
  {
    return (pc >> 8) == (address >> 8) ? 2 : 3;
  }

  void store_absolute(int data)
  {
    int address = memory_bus->read16(pc);
    pc += 2;
    memory_bus->write(address, data);
  }

  void store_absolute_x(int data)
  {
    int address = memory_bus->read16(pc) + reg_x;
    pc += 2;
    memory_bus->write(address, data);
  }

  void store_absolute_y(int data)
  {
    int address = memory_bus->read16(pc) + reg_y;
    pc += 2;
    memory_bus->write(address, data);
  }

  void store_zero_page(int data)
  {
    int address = memory_bus->read(pc++);
    memory_bus->write(address, data);
  }

  void store_zero_page_x(int data)
  {
    int address = memory_bus->read(pc++) + reg_x;
    memory_bus->write(address, data);
  }

  void store_zero_page_y(int data)
  {
    int address = memory_bus->read(pc++) + reg_x;
    memory_bus->write(address, data);
  }

  void store_indirect_x(int data)
  {
    int address;
    address = memory_bus->read(pc++) + reg_x;
    address = memory_bus->read16(address & 0xff);
    memory_bus->write(address, data);
  }

  void store_indirect_y(int data)
  {
    int address;
    address = memory_bus->read(pc++);
    address = memory_bus->read16(address) + reg_y;
    memory_bus->write(address, data);
  }

  bool same_page(int address)
  {
    return (pc >> 8) == (address >> 8);
  }

  MemoryBus *memory_bus;
  bool running;

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

