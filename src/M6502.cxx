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

#include "Disassembler.h"
#include "M6502.h"

M6502::M6502() :
  running{true},
  reg_a{0},
  reg_x{0},
  reg_y{0},
  total_cycles{0},
  total_instructions{0},
  breakpoint{0xffff},
  debug{false}
{
}

M6502::~M6502()
{
}

void M6502::reset()
{
  reg_a = 0;
  reg_x = 0;
  reg_y = 0;

  pc = memory_bus->read16(0xfffe);
  sp = 0xff;
  total_cycles = 0;
  total_instructions = 0;
}

void M6502::dump()
{
  printf(" PC: 0x%04x, SP: 0x%04x, A: 0x%02x, X: 0x%02x, Y: 0x%02x\n",
    pc, sp, reg_a, reg_x, reg_y);
  printf(" N V B * D I Z C\n");
  printf(" %d %d %d %d %d %d %d %d\n",
    status.n,
    status.v,
    status.b,
    status.one,
    status.d,
    status.i,
    status.z,
    status.c);
  printf(" total_instructions=%d\n", total_instructions);
  printf(" total_cycles=%d\n", total_cycles);
}

void M6502::illegal_instruction(uint8_t opcode)
{
  // FIXME: Print the intruction.
  printf("Illegal Instruction: 0x%02x\n", opcode);
  dump();
  exit(-1);
}

int M6502::step()
{
  int cycles = execute_instruction();

  total_cycles += cycles;
  total_instructions++;

  return cycles;
}

int M6502::execute_instruction()
{
  int address = pc++;
  uint8_t opcode = memory_bus->read(address);
  //int cycles = 0;
  int a, data;
  int8_t offset;

  if (debug)
  {
    char text[16];
    uint8_t code[3];
    code[0] = memory_bus->read(address + 0);
    code[1] = memory_bus->read(address + 1);
    code[2] = memory_bus->read(address + 2);

    Disassembler::disassemble(code, address, text);

    printf(" --- 0x%04x: %02x - %s ---\n", address, opcode, text);
  }

  if (address == breakpoint)
  {
    status.b = 1;
    stop();
  }

  switch (opcode)
  {
    case 0x00:     //  BRK
      status.b = 1;
      stop();
      return 7;

    case 0x01:     //  ORA (Indirect, X)
      data = read_indirect_x(a);
      run_or(data);
      return 6;

    case 0x05:     //  ORA Zero Page
      data = read_zero_page(a);
      run_or(data);
      return 3;

    case 0x06:     //  ASL Zero Page
      data = read_zero_page(a);
      run_asl_memory(a, data);
      return 5;

    case 0x08:     //  PHP
      push(status.reg_p);
      return 3;

    case 0x09:     //  ORA #Immediate
      data = read_immediate();
      run_or(data);
      return 2;

    case 0x0a:     //  ASL <Accumulator>
      reg_a <<= 1;
      set_flags(reg_a);
      return 2;

    case 0x0d:     //  ORA Absolute
      data = read_absolute(a);
      run_or(data);
      return 4;

    case 0x0e:     //  ASL Absolute
      data = read_absolute(a);
      run_asl_memory(a, data);
      return 6;

    case 0x10:     //  BPL
      offset = read_immediate();
      a = pc;

      if (status.n == 0)
      {
        pc += offset;
        return get_branch_cycles(a);
      }

      return 2;

    case 0x11:     //  ORA (Indirect), Y
      data = read_indirect_y(a);
      run_or(data);
      return same_page(a) ? 5 : 6;

    case 0x15:     //  ORA Zero Page, X
      data = read_zero_page_x(a);
      run_or(data);
      return 4;

    case 0x16:     //  ASL Zero Page, X
      data = read_zero_page_x(a);
      run_asl_memory(a, data);
      return 6;

    case 0x18:     //  CLC
      status.c = 0;
      return 2;

    case 0x19:     //  ORA Absolute, Y
      data = read_absolute_y(a);
      run_or(data);
      return same_page(a) ? 4 : 5;

    case 0x1d:     //  ORA Absolute, X
      data = read_absolute_x(a);
      run_or(data);
      return same_page(a) ? 4 : 5;

    case 0x1e:     //  ASL Absolute, X
      data = read_absolute_x(a);
      run_asl_memory(a, data);
      return 7;

    case 0x20:     //  JSR
      data = read_address();
      push(pc >> 8);
      push(pc & 0xff);
      pc = data;
      return 6;

    case 0x21:     //  AND (Indirect, X)
      data = read_indirect_x(a);
      run_and(data);
      return 6;

    case 0x24:     //  BIT Zero Page
      data = read_zero_page(a);
      run_bit(data);
      return 3;

    case 0x25:     //  AND Zero Page
      data = read_zero_page(a);
      run_and(data);
      return 3;

    case 0x26:     //  ROL Zero Page
      data = read_zero_page(a);
      run_rol_memory(a, data);
      return 5;

    case 0x28:     //  PLP
      status.reg_p = pop();
      return 4;

    case 0x29:     //  AND #Immediate
      data = read_immediate();
      run_and(data);
      return 2;

    case 0x2a:     //  ROL <Accumulator>
      reg_a = reg_a << 1;
      set_flags(reg_a);
      reg_a |= status.c;
      status.z = reg_a == 0;
      return 2;

    case 0x2c:     //  BIT Absolute
      data = read_absolute(a);
      run_bit(data);
      return 4;

    case 0x2d:     //  AND Absolute
      data = read_absolute(a);
      run_and(data);
      return 4;

    case 0x2e:     //  ROR Absolute
      data = read_absolute(a);
      run_ror_memory(a, data);
      return 6;

    case 0x30:     //  BMI
      offset = read_immediate();
      a = pc;

      if (status.n == 1)
      {
        pc += offset;

        return get_branch_cycles(a);
      }

      return 2;

    case 0x31:     //  AND (Indirect), Y
      data = read_indirect_y(a);
      run_and(data);
      return same_page(a) ? 5 : 6;

    case 0x35:     //  AND Zero Page, X
      data = read_zero_page_x(a);
      run_and(data);
      return 4;

    case 0x36:     //  ROR Zero Page, X
      data = read_zero_page_x(a);
      run_ror_memory(a, data);
      return 6;

    case 0x38:     //  SEC
      status.c = 1;
      return 2;

    case 0x39:     //  AND Absolute, Y
      data = read_absolute_y(a);
      run_and(data);
      return same_page(a) ? 4 : 5;

    case 0x3d:     //  AND Absolute, X
      data = read_absolute_x(a);
      run_and(data);
      return same_page(a) ? 4 : 5;

    case 0x3e:     //  ROR Absolute, X
      data = read_absolute_x(a);
      run_ror_memory(a, data);
      return 7;

    case 0x40:     //  RTI
      status.reg_p = pop();
      pc = pop();
      pc |= pop() << 8;
      return 6;

    case 0x41:     //  EOR (Indirect, X)
      data = read_indirect_x(a);
      run_eor(data);
      return 6;

    case 0x45:     //  EOR Zero Page
      data = read_zero_page(a);
      run_eor(data);
      return 3;

    case 0x46:     //  LSR Zero Page
      data = read_zero_page(a);
      run_lsr_memory(a, data);
      return 5;

    case 0x48:     //  PHA
      push(reg_a);
      return 3;

    case 0x49:     //  EOR #Immediate
      data = read_immediate();
      run_eor(data);
      return 2;

    case 0x4a:     //  LSR <Accumulator>
      status.c = (reg_a & 1) != 0;
      reg_a = (reg_a >> 1) & 0xff;
      status.z = reg_a == 0;
      status.n = (reg_a & 0x80) != 0;
      return 2;

    case 0x4c:     //  JMP Absolute
      pc = read_address();
      return 3;

    case 0x4d:     //  EOR Absolute
      data = read_absolute(a);
      run_eor(data);
      return 4;

    case 0x4e:     //  LSR Absolute
      data = read_absolute(a);
      run_lsr_memory(a, data);
      return 6;

    case 0x50:     //  BVC
      offset = read_immediate();
      a = pc;

      if (status.v == 0)
      {
        pc += offset;
        // FIXME: Is BVC 3 cycles no matter the page?
        return get_branch_cycles(a);
      }

      return 2;

    case 0x51:     //  EOR (Indirect), Y
      data = read_indirect_y(a);
      run_eor(data);
      return same_page(a) ? 5 : 6;

    case 0x55:     //  EOR Zero Page, X
      data = read_zero_page_x(a);
      run_eor(data);
      return 4;

    case 0x56:     //  LSR Zero Page, X
      data = read_zero_page_x(a);
      run_lsr_memory(a, data);
      return 6;

    case 0x58:     //  CLI
      status.i = 0;
      return 2;

    case 0x59:     //  EOR Absolute, Y
      data = read_absolute_y(a);
      run_eor(data);
      return same_page(a) ? 4 : 5;

    case 0x5d:     //  EOR Absolute, X
      data = read_absolute_x(a);
      run_eor(data);
      return same_page(a) ? 4 : 5;

    case 0x5e:     //  LSR Absolute, X
      data = read_absolute_x(a);
      run_lsr_memory(a, data);
      return 7;

    case 0x60:     //  RTS
      pc = pop();
      pc |= pop() << 8;
      return 6;

    case 0x61:     //  ADC (Indirect, X)
      data = read_indirect_x(a);
      run_adc(data);
      return 6;

    case 0x65:     //  ADC ZeroPage
      data = read_zero_page(a);
      run_adc(data);
      return 3;

    case 0x66:     //  ROR Zero Page
      data = read_zero_page(a);
      run_ror_memory(a, data);
      return 5;

    case 0x68:     //  PLA
      reg_a = pop();
      return 4;

    case 0x69:     //  ADC #Immediate
      data = read_immediate();
      run_adc(data);
      return 2;

    case 0x6a:     //  ROR <Accumulator>
      run_ror();
      return 2;

    case 0x6c:     //  JMP (Indirect)
      data = read_address();
      pc = memory_bus->read16(pc);
      return 5;

    case 0x6d:     //  ADC Absolute
      data = read_absolute(a);
      run_adc(data);
      return 4;

    case 0x6e:     //  ROL Absolute
      data = read_absolute(a);
      run_rol_memory(a, data);
      return 6;

    case 0x70:     //  BVS
      offset = read_immediate();
      a = pc;

      if (status.v == 1)
      {
        pc += offset;

        return get_branch_cycles(a);
      }

      return 2;

    case 0x71:     //  ADC (Indirect), Y
      data = read_indirect_y(a);
      run_adc(data);
      return same_page(a) ? 5 : 6;

    case 0x75:     //  ADC ZeroPage, X
      data = read_zero_page_x(a);
      run_adc(data);
      return 4;

    case 0x76:     //  ROL Zero Page, X
      data = read_zero_page_x(a);
      run_rol_memory(a, data);
      return 6;

    case 0x78:     //  SEI
      status.i = 1;
      return 2;

    case 0x79:     //  ADC Absolute, Y
      data = read_absolute_y(a);
      run_adc(data);
      return same_page(a) ? 4 : 5;

    case 0x7d:     //  ADC Absolute, X
      data = read_absolute_x(a);
      run_adc(data);
      return same_page(a) ? 4 : 5;

    case 0x7e:     //  ROL Absolute, X
      data = read_absolute_x(a);
      run_rol_memory(a, data);
      return 7;

    case 0x81:     //  STA (Indirect, X)
      store_indirect_x(reg_a);
      return 6;

    case 0x84:     //  STY Zero Page
      store_zero_page(reg_y);
      return 3;

    case 0x85:     //  STA Zero Page
      store_zero_page(reg_a);
      return 3;

    case 0x86:     //  STX Zero Page
      store_zero_page(reg_x);
      return 3;

    case 0x88:     //  DEY <Implied>
      reg_y--;
      set_flags(reg_y);
      return 2;

    case 0x8a:     //  TXA
      reg_a = reg_x;
      set_load_flags(reg_a);
      return 2;

    case 0x8c:     //  STY Absolute
      store_absolute(reg_y);
      return 4;

    case 0x8d:     //  STA Absolute
      store_absolute(reg_a);
      return 4;

    case 0x8e:     //  STX Absolute
      store_absolute(reg_x);
      return 4;

    case 0x90:     //  BCC
      offset = read_immediate();
      a = pc;

      if (status.c == 0)
      {
        pc += offset;
        return get_branch_cycles(a);
      }

      return 2;

    case 0x91:     //  STA (Indirect), Y
      store_indirect_y(reg_a);
      return 6;

    case 0x94:     //  STY Zero Page, X
      store_zero_page_x(reg_y);
      return 4;

    case 0x95:     //  STA Zero Page, X
      store_zero_page_x(reg_a);
      return 4;

    case 0x96:     //  STX Zero Page, Y
      store_zero_page_y(reg_x);
      return 4;

    case 0x98:     //  TYA
      reg_a = reg_y;
      set_load_flags(reg_a);
      return 2;

    case 0x99:     //  STA Absolute, Y
      store_absolute_y(reg_a);
      return 5;

    case 0x9a:     //  TXS
      sp = reg_x;
      return 2;

    case 0x9d:     //  STA Absolute, X
      store_absolute_x(reg_a);
      return 5;

    case 0xa0:     //  LDY #Immediate
      reg_y = read_immediate();
      set_load_flags(reg_y);
      return 2;

    case 0xa1:     //  LDA (Indirect, X)
      data = read_indirect_x(a);
      reg_a = data;
      set_load_flags(reg_a);
      return 6;

    case 0xa2:     //  LDX #Immediate
      reg_x = read_immediate();
      set_load_flags(reg_x);
      return 2;

    case 0xa4:     //  LDY Zero Page
      reg_y = read_zero_page(a);
      set_load_flags(reg_y);
      return 3;

    case 0xa5:     //  LDA Zero Page
      reg_a = read_zero_page(a);
      set_load_flags(reg_a);
      return 3;

    case 0xa6:     //  LDX Zero Page
      reg_x = read_zero_page(a);
      set_load_flags(reg_x);
      return 3;

    case 0xa8:     //  TAY
      reg_y = reg_a;
      set_load_flags(reg_y);
      return 2;

    case 0xa9:     //  LDA #Immediate
      data = read_immediate();
      reg_a = data;
      set_load_flags(reg_a);
      return 2;

    case 0xaa:     //  TAX
      reg_x = reg_a;
      set_load_flags(reg_x);
      return 2;

    case 0xac:     //  LDY Absolute
      reg_y = read_absolute(a);
      set_load_flags(reg_y);
      return 4;

    case 0xad:     //  LDA Absolute
      reg_a = read_absolute(a);
      set_load_flags(reg_a);
      return 4;

    case 0xae:     //  LDX Absolute
      reg_x = read_absolute(a);
      set_load_flags(reg_x);
      return 4;

    case 0xb0:     //  BCS
      offset = read_immediate();
      a = pc;

      if (status.c == 1)
      {
        pc += offset;
        return get_branch_cycles(a);
      }

      return 2;

    case 0xb1:     //  LDA (Indirect), Y
      reg_a = read_indirect_y(a);
      set_load_flags(reg_a);
      return same_page(a) ? 5 : 6;

    case 0xb4:     //  LDY Zero Page, X
      reg_y = read_zero_page_x(a);
      set_load_flags(reg_y);
      return 4;

    case 0xb5:     //  LDA Zero Page, X
      reg_a = read_zero_page_x(a);
      set_load_flags(reg_a);
      return 4;

    case 0xb6:     //  LDX Zero Page, Y
      reg_x = read_zero_page_y();
      set_load_flags(reg_x);
      return 4;

    case 0xb8:     //  CLV
      status.v = 0;
      return 2;

    case 0xb9:     //  LDA Absolute, Y
      reg_a = read_absolute_y(a);
      set_load_flags(reg_a);
      return same_page(a) ? 4 : 5;

    case 0xba:     //  TSX
      reg_x = sp;
      set_flags(reg_x);
      return 2;

    case 0xbc:     //  LDY Absolute, X
      reg_y = read_absolute_x(a);
      set_load_flags(reg_y);
      return same_page(a) ? 4 : 5;

    case 0xbd:     //  LDA Absolute, X
      reg_a = read_absolute_x(a);
      set_load_flags(reg_a);
      return same_page(a) ? 4 : 5;

    case 0xbe:     //  LDX Absolute, Y
      reg_x = read_absolute_y(a);
      set_load_flags(reg_x);
      return same_page(a) ? 4 : 5;

    case 0xc0:     //  CPY Immediate
      data = read_immediate();
      run_compare(reg_y, data);
      return 2;

    case 0xc1:     //  CMP (Indirect, X)
      data = read_indirect_x(a);
      run_compare(reg_a, data);
      return 6;

    case 0xc4:     //  CPY Zero Page
      data = read_zero_page(a);
      run_compare(reg_y, data);
      return 3;

    case 0xc5:     //  CMP Zero Page
      data = read_zero_page(a);
      run_compare(reg_a, data);
      return 3;

    case 0xc6:     //  DEC Zero Page
      data = read_zero_page(a);
      run_dec_memory(a, data);
      return 5;

    case 0xc8:     //  INY <Implied>
      reg_y = (reg_y + 1) & 0xff;
      set_load_flags(reg_y);
      return 2;

    case 0xc9:     //  CMP #Immediate
      data = read_immediate();
      run_compare(reg_a, data);
      return 3;

    case 0xca:     //  DEX
      reg_x = (reg_x - 1) & 0xff;
      set_load_flags(reg_x);
      return 2;

    case 0xcc:     //  CPY Absolute
      data = read_absolute(a);
      run_compare(reg_y, data);
      return 4;

    case 0xcd:     //  CMP Absolute
      data = read_absolute(a);
      run_compare(reg_a, data);
      return 4;

    case 0xce:     //  DEC Absolute
      data = read_absolute(a);
      run_dec_memory(a, data);
      return 6;

    case 0xd0:     //  BNE
      offset = read_immediate();
      a = pc;

      if (status.z == 0)
      {
        pc += offset;
        return get_branch_cycles(a);
      }

      return 2;

    case 0xd1:     //  CMP (Indirect), Y
      data = read_indirect_y(a);
      run_compare(reg_a, data);
      return same_page(a) ? 5 : 6;

    case 0xd5:     //  CMP Zero Page, X
      data = read_zero_page_x(a);
      run_compare(reg_a, data);
      return 4;

    case 0xd6:     //  DEC Zero Page, X
      data = read_zero_page_x(a);
      run_dec_memory(a, data);
      return 6;

    case 0xd8:     //  CLD
      status.d = 0;
      return 2;

    case 0xd9:     //  CMP Absolute, Y
      data = read_absolute_y(a);
      run_compare(reg_a, data);
      return same_page(a) ? 4 : 5;

    case 0xdd:     //  CMP Absolute, X
      data = read_absolute_x(a);
      run_compare(reg_a, data);
      return same_page(a) ? 4 : 5;

    case 0xde:     //  DEC Absolute, X
      data = read_absolute_x(a);
      run_dec_memory(a, data);
      return 7;

    case 0xe0:     //  CPX #Immediate
      data = read_immediate();
      run_compare(reg_x, data);
      return 2;

    case 0xe1:     //  SBC (Indirect, X)
      data = read_indirect_x(a);
      run_sbc(data);
      return 6;

    case 0xe4:     //  CPX Zero Page
      data = read_zero_page(a);
      run_compare(reg_x, data);
      return 3;

    case 0xe5:     //  SBC Zero Page
      data = read_zero_page(a);
      run_sbc(data);
      return 3;

    case 0xe6:     //  INC Zero Page
      data = read_zero_page(a);
      run_inc_memory(a, data);
      return 5;

    case 0xe8:     //  INX (Implied)
      reg_x = (reg_x + 1) & 0xff;
      set_load_flags(reg_x);
      return 2;

    case 0xe9:     //  SBC #Immediate
      data = read_immediate();
      run_sbc(data);
      return 2;

    case 0xea:     //  NOP
      return 2;

    case 0xec:     //  CPX Absolute
      data = read_absolute(a);
      run_compare(reg_x, data);
      return 4;

    case 0xed:     //  SBC Absolute
      data = read_absolute(a);
      run_sbc(data);
      return 4;

    case 0xee:     //  INC Absolute
      data = read_absolute(a);
      run_inc_memory(a, data);
      return 6;

    case 0xf0:     //  BEQ
      offset = read_immediate();
      a = pc;

      if (status.z == 1)
      {
        pc += offset;
        return get_branch_cycles(a);
      }

      return 2;

    case 0xf1:     //  SBC (Indirect), Y
      data = read_indirect_y(a);
      run_sbc(data);
      return same_page(a) ? 5 : 6;

    case 0xf5:     //  SBC Zero Page, X
      data = read_zero_page_x(a);
      run_sbc(data);
      return 4;

    case 0xf6:     //  INC Zero Page, X
      data = read_zero_page_x(a);
      run_inc_memory(a, data);
      return 6;

    case 0xf8:     //  SED
      status.d = 1;
      return 2;

    case 0xf9:     //  SBC Absolute, Y
      data = read_absolute_y(a);
      run_sbc(data);
      return same_page(a) ? 4 : 5;

    case 0xfd:     //  SBC Absolute, X
      data = read_absolute_x(a);
      run_sbc(data);
      return same_page(a) ? 4 : 5;

    case 0xfe:     //  INC Absolute, X
      data = read_absolute_x(a);
      run_inc_memory(a, data);
      return 7;

    default:
      illegal_instruction(opcode);
  }

#if 0
  PC++;
  if (count_cycles==1) cycles++;

  bus.clock(cycles);
#endif

  return 0;
}

