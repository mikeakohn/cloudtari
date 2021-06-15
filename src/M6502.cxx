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

#include "M6502.h"

M6502::M6502() : debug(false)
{
  reset();
}

M6502::~M6502()
{
}

void M6502::reset()
{
  reg_a = 0;
  reg_x = 0;
  reg_y = 0;

  pc = 0;
  sp = 0xff;
  total_cycles = 0;
  total_instructions = 0;
}

void M6502::dump()
{
  printf("----- 6502 -----\n");
  printf(" PC=0x%04x SP=0x%04x\n", pc, sp);
  printf(" A=%d X=%d Y=%d\n", reg_a, reg_x, reg_y);
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

int M6502::execute_instruction()
{
  int address = pc++;
  uint8_t opcode = memory_bus->read(address);
  //int cycles = 0;
  int a, data;

  if (debug) { printf("0x%04x %02x", address, opcode); }

  switch (opcode)
  {
    case 0x00:     //  BRK
      status.b = 1;
      stop();
      return 7;

    case 0x01:     //  ORA ((Indirect, X))
      data = read_indirect_x();
      reg_a = reg_a | data;
      set_flags(reg_a);
      return 6;

    case 0x05:     //  ORA (Zero Page)
      data = read_zero_page();
      reg_a = reg_a | data;
      set_flags(reg_a);
      return 3;

    case 0x06:     //  ASL (Zero Page)
      a = pc;
      data = read_zero_page();
      data = data << 1;
      set_flags(data);
      memory_bus->write(a, data);
      return 5;

    case 0x08:     //  PHP
      push(status.reg_p);
      return 3;

    case 0x09:     //  ORA (Immediate)
      data = read_immediate();
      reg_a = reg_a ^ data;
      set_flags(reg_a);
      return 2;

    case 0x0a:     //  ASL (Accumulator)
      reg_a <<= 1;
      set_flags(reg_a);
      return 2;

    case 0x0d:     //  ORA (Absolute)
      data = read_absolute();
      reg_a |= data;
      set_flags(reg_a);
      return 4;

    case 0x0e:     //  ASL (Absolute)
      data = read_absolute(a);
      data = data << 1;
      set_flags(data);
      memory_bus->write(a, data);
      return 6;

#if 0
    case 0x10:     //  BPL
      branch=(byte)bus.read_mem(PC+1);

      if (N==0)
      {
        PC=PC+branch;
        if (count_cycles==1)
        {
          if ((PC&255)!=((PC-branch)&255))
          { cycles=3; }
          else
          { cycles=2; }
        }
      }
        else
      if (count_cycles==1) cycles=2;

      PC++;
      break;

    case 0x11:     //  ORA ((Indirect), Y)
      M=bus.read_mem(++PC);
      M=bus.read_mem(M)+(bus.read_mem(M+1)<<8)+Y;
      A=A|bus.read_mem(M);

      Z=A;
      N=(byte)(A&128);
      if (count_cycles==1) cycles=5;
      break;

    case 0x15:     //  ORA (Zero Page, X)
      M=bus.read_mem(++PC)+X;
      A=A|bus.read_mem(M);

      Z=A;
      N=(byte)(A&128);
      if (count_cycles==1) cycles=4;
      break;

    case 0x16:     //  ASL (Zero Page, X)
      M=bus.read_mem(++PC)+X;
      C=(byte)((bus.read_mem(M)&128)>>7);
      bus.write_mem(M,(char)((bus.read_mem(M)<<1)&255));

      Z=bus.read_mem(M);
      N=(byte)(bus.read_mem(M)&128);
      if (count_cycles==1) cycles=6;
      break;

    case 0x18:     //  CLC
      status.c = 0;
      return 2;

    case 0x19:     //  ORA (Absolute, Y)
      M=bus.read_mem(++PC);
      M=M+(bus.read_mem(++PC)<<8)+Y;
      A=A|bus.read_mem(M);

      Z=A;
      N=(byte)(A&128);
      if (count_cycles==1)
      {
        if ((M&255)!=((M-Y)&255))
        { cycles=5; }
        else
        { cycles=4; }
      }
      break;

    case 0x1d:     //  ORA (Absolute, X)
      M=bus.read_mem(++PC);
      M=M+(bus.read_mem(++PC)<<8)+X;
      A=A|bus.read_mem(M);

      Z=A;
      N=(byte)(A&128);
      if (count_cycles==1)
      {
        if ((M&255)!=((M-X)&255))
        { cycles=5; }
        else
        { cycles=4; }
      }
      break;

    case 0x1e:     //  ASL (Absolute, X)

          M=bus.read_mem(++PC);
          M=(bus.read_mem(++PC)<<8)+X;
          C=(byte)((bus.read_mem(M)&128)>>7);
          bus.write_mem(M,(char)((bus.read_mem(M)<<1)&255));

          Z=bus.read_mem(M);
          N=(byte)(bus.read_mem(M)&128);
          if (count_cycles==1) cycles=7;
          break;

    case 0x20:     //  JSR
          bus.write_mem(SP--,(char)(((PC+2)>>8)&(int)255));
          bus.write_mem(SP--,(char)((PC+2)&(int)255));
          PC=(bus.read_mem(PC+2)<<8)+bus.read_mem(PC+1);
          if (count_cycles==1) cycles=6;
          continue;

    case 0x21:     //  AND ((Indirect, X))
          M=bus.read_mem(++PC)+X;
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8);
          A=A&bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=6;
          break;

    case 0x24:     //  BIT (Zero Page)

          temp=(A&bus.read_mem(++PC));

          Z=temp;
          N=(byte)(temp&128);
          V=(byte)(temp&64);
          if (count_cycles==1) cycles=3;
          break;

    case 0x25:     //  AND (Zero Page)

          M=bus.read_mem(++PC);
          A=A&bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=3;
          break;

    case 0x26:     //  ROL (Zero Page)

          M=++PC;
          temp=(byte)(bus.read_mem(M)&128);
          bus.write_mem(M,(char)((bus.read_mem(M)<<1)+C));

          C=(byte)(temp>>7);
          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=5;
          break;

    case 0x28:     //  PLP

          P=bus.read_mem(++SP);
          unload_P();
          if (count_cycles==1) cycles=4;
          break;

    case 0x29:     //  AND (Immediate)

          A=A&bus.read_mem(++PC);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=2;
          break;

    case 0x2a:     //  ROL (Accumulator)

          temp=(byte)(A&128);
          A=(A<<1)+C;

          C=(byte)(temp>>7);
          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=2;
          break;

    case 0x2c:     //  BIT (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          temp=(A&bus.read_mem(M));

          Z=temp;
          N=(byte)(temp&128);
          V=(byte)(temp&64);
          if (count_cycles==1) cycles=4;
          break;

    case 0x2d:     //  AND (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          A=A&bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=4;
          break;

    case 0x2e:     //  ROR (Absolute)

          M=(++PC);
          M=M+((++PC)<<8);
          temp=(byte)(bus.read_mem(M)&1);
          bus.write_mem(M,(char)((bus.read_mem(M)>>1)+(C<<7)));

          C=(byte)temp;
          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=6;
          break;

    case 0x30:     //  BMI
          branch=(byte)bus.read_mem(PC+1);

          if (N!=0)
          {
            PC=PC+branch;
            if (count_cycles==1)
            {
              if ((PC&255)!=((PC-branch)&255))
              { cycles=3; }
              else
              { cycles=2; }
            }
          }
            else
          if (count_cycles==1) cycles=2;

          PC++;
          break;

    case 0x31:     //  AND ((Indirect, Y))
          M=bus.read_mem(++PC);
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8)+Y;
          A=A&bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=6; }
            else
            { cycles=5; }
          }
          break;

    case 0x35:     //  AND (Zero Page, X)

          M=bus.read_mem(++PC)+X;
          A=A&bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=4;
          break;

    case 0x36:     //  ROR (Zero Page, X)

          M=(++PC)+X;
          temp=(byte)(bus.read_mem(M)&1);
          bus.write_mem(M,(char)((bus.read_mem(M)>>1)+(C<<7)));

          C=(byte)temp;
          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=6;
          break;

    case 0x38:     //  SEC
      status.c = 1;
      return 2;

    case 0x39:     //  AND (Absolute, Y)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+Y;
          A=A&bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1)
          {
            if ((M&255)!=(M-Y))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

    case 0x3d:     //  AND (Absolute, X)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+X;
          A=A&bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-X)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0x3e:     //  ROR (Absolute, X)

          M=(++PC);
          M=M+((++PC)<<8)+X;
          temp=(byte)(bus.read_mem(M)&1);
          bus.write_mem(M,(char)((bus.read_mem(M)>>1)+(C<<7)));

          C=(byte)temp;
          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=7;
          break;

        case 0x40:     //  RTI

          P=bus.read_mem(++SP);
          unload_P();
          PC=bus.read_mem(++SP);
          PC=PC+((bus.read_mem(++SP))<<8);
          if (count_cycles==1) cycles=6;
          break;

        case 0x41:     //  EOR ((Indirect, X))
          M=bus.read_mem(++PC)+X;
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8);
          A=A^bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=6;
          break;

        case 0x45:     //  EOR (Zero Page)

          M=bus.read_mem(++PC);
          A=A^bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=3;
          break;

        case 0x46:     //  LSR (Zero Page)

          M=bus.read_mem(++PC);
          C=(byte)(bus.read_mem(M)&1);
          bus.write_mem(M,(char)(bus.read_mem(M)>>1));

          Z=bus.read_mem(M);
          N=0;
          if (count_cycles==1) cycles=5;
          break;

        case 0x48:     //  PHA
          bus.write_mem(SP--,(char)A);
          if (count_cycles==1) cycles=3;
          break;

        case 0x49:     //  EOR (Immediate)
          A=A^bus.read_mem(++PC);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0x4a:     //  LSR (Accumulator)
          C=(byte)(A&1);
          A=((A>>1)&255);

          Z=A;
          N=0;
          if (count_cycles==1) cycles=2;
          break;

        case 0x4c:     //  JMP (Absolute)
          PC=bus.read_mem(PC+1)+(bus.read_mem(PC+2)<<8);
          if (count_cycles==1) cycles=3;
          continue;

        case 0x4d:     //  EOR (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          A=A^bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=4;
          break;

        case 0x4e:     //  LSR (Absolute)

          M=bus.read_mem(++PC);
          M=(bus.read_mem(++PC)<<8);
          C=(byte)(bus.read_mem(M)&1);
          bus.write_mem(M,(char)(bus.read_mem(M)>>1));

          Z=bus.read_mem(M);
          N=0;
          if (count_cycles==1) cycles=6;
          break;

        case 0x50:     //  BVC
          branch=(byte)bus.read_mem(PC+1);

          if (V==0)
          {
            PC=PC+branch;
            if (count_cycles==1)
            {
              if ((PC&255)!=((PC-branch)&255))
              { cycles=3; }
              else
              { cycles=2; }
            }
          }
            else
          if (count_cycles==1) cycles=2;

          PC++;
          break;

        case 0x51:     //  EOR ((Indirect, Y))
          M=bus.read_mem(++PC);
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8)+Y;
          A=A^bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=6; }
            else
            { cycles=5; }
          }
          break;

        case 0x55:     //  EOR (Zero Page, X)

          M=bus.read_mem(++PC)+X;
          A=A^bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=4;
          break;

        case 0x56:     //  LSR (Zero Page, X)

          M=bus.read_mem(++PC)+X;
          C=(byte)(bus.read_mem(M)&1);
          bus.write_mem(M,(char)(bus.read_mem(M)>>1));

          Z=bus.read_mem(M);
          N=0;
          if (count_cycles==1) cycles=6;
          break;

    case 0x58:     //  CLI
      status.i = 0;
      return 2; 

    case 0x59:     //  EOR (Absolute, Y)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+Y;
          A=A^bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0x5d:     //  EOR (Absolute, X)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+X;
          A=A^bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-X)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0x5e:     //  LSR (Absolute, X)

          M=bus.read_mem(++PC);
          M=(bus.read_mem(++PC)<<8)+X;
          C=(byte)(bus.read_mem(M)&1);
          bus.write_mem(M,(char)(bus.read_mem(M)>>1));

          Z=bus.read_mem(M);
          N=0;
          if (count_cycles==1) cycles=7;
          break;

        case 0x60:     //  RTS

          PC=bus.read_mem(++SP);
          PC=PC+((bus.read_mem(++SP))<<8);
          if (count_cycles==1) cycles=6;
          break;

        case 0x61:     //  ADC ((Indirect, X))
          M=bus.read_mem(++PC)+X;
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8);
          A=A+bus.read_mem(M)+C;
          if (A>255)
          {
            C=1;
            A=A-256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1) cycles=6;
          break;

        case 0x65:     //  ADC (ZeroPage)
          M=bus.read_mem(++PC);
          A=A+bus.read_mem(M)+C;
          if (A>255)
          {
            C=1;
            A=A-256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1) cycles=3;
          break;

        case 0x66:     //  ROR (Zero Page)

          M=++PC;
          temp=(byte)(bus.read_mem(M)&1);
          bus.write_mem(M,(char)((bus.read_mem(M)>>1)+(C<<7)));

          C=(byte)temp;
          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=5;
          break;

        case 0x68:     //  PLA

          A=bus.read_mem(++SP);
          if (count_cycles==1) cycles=4;
          break;

        case 0x69:     //  AND (Immediate)
          A=A&bus.read_mem(++PC);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0x6a:     //  ROR (Accumulator)

          temp=(byte)(A&1);
          A=(A>>1)+(C<<7);

          C=(byte)temp;
          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0x6c:     //  JMP (Indirect)
          M=bus.read_mem(PC+1)+(bus.read_mem(PC+2)<<8);
          PC=bus.read_mem(M)+(bus.read_mem(M+1)<<8);
          if (count_cycles==1) cycles=5;
          continue;

        case 0x6d:     //  ADC (Absolute)
          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          A=A+bus.read_mem(M)+C;
          if (A>255)
          {
            C=1;
            A=A-256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1) cycles=4;
          break;

        case 0x6e:     //  ROL (Absolute)

          M=(++PC);
          M=M+((++PC)<<8);
          temp=(byte)(bus.read_mem(M)&128);
          bus.write_mem(M,(char)((bus.read_mem(M)<<1)+C));

          C=(byte)(temp>>7);
          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=6;
          break;

        case 0x70:     //  BVS
          branch=(byte)bus.read_mem(PC+1);

          if (V!=0)
          {
            PC=PC+branch;
            if (count_cycles==1)
            {
              if ((PC&255)!=((PC-branch)&255))
              { cycles=3; }
              else
              { cycles=2; }
            }
          }
            else
          if (count_cycles==1) cycles=2;

          PC++;
          break;

        case 0x71:     //  ADC ((Indirect, Y))
          M=bus.read_mem(++PC);
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8)+Y;
          A=A+bus.read_mem(M)+C;
          if (A>255)
          {
            C=1;
            A=A-256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=6; }
            else
            { cycles=5; }
          }
          break;

        case 0x75:     //  ADC (ZeroPage, X)
          M=bus.read_mem(++PC)+X;
          A=A+bus.read_mem(M)+C;
          if (A>255)
          {
            C=1;
            A=A-256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1) cycles=4;
          break;

        case 0x76:     //  ROL (Zero Page, X)

          M=(++PC)+X;
          temp=(byte)(bus.read_mem(M)&128);
          bus.write_mem(M,(char)((bus.read_mem(M)<<1)+C));

          C=(byte)(temp>>7);
          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=6;
          break;

        case 0x78:     //  SEI
          I=1;
          if (count_cycles==1) cycles=2;
          break;

        case 0x79:     //  ADC (Absolute, Y)
          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+Y;
          A=A+bus.read_mem(M)+C;
          if (A>255)
          {
            C=1;
            A=A-256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0x7d:     //  ADC (Absolute, X)
          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+X;
          A=A+bus.read_mem(M)+C;
          if (A>255)
          {
            C=1;
            A=A-256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-X)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0x7e:     //  ROL (Absolute, X)

          M=(++PC);
          M=M+((++PC)<<8)+X;
          temp=(byte)(bus.read_mem(M)&128);
          bus.write_mem(M,(char)((bus.read_mem(M)<<1)+C));

          C=(byte)(temp>>7);
          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=7;
          break;

        case 0x81:     //  STA ((Indirect, X))

          M=bus.read_mem(++PC)+X;
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8);
          bus.write_mem(M,(char)A);
          if (count_cycles==1) cycles=6;
          break;

        case 0x84:     //  STY (Zero Page)

          M=bus.read_mem(++PC);
          bus.write_mem(M,(char)Y);
          if (count_cycles==1) cycles=3;
          break;

        case 0x85:     //  STA (Zero Page)

          M=bus.read_mem(++PC);
          bus.write_mem(M,(char)A);
          if (count_cycles==1) cycles=3;
          break;

        case 0x86:     //  STX (Zero Page)

          M=bus.read_mem(++PC);
          bus.write_mem(M,(char)X);
          if (count_cycles==1) cycles=3;
          break;

        case 0x88:     //  DEY (Implied)
          Y--;
          if (Y<0) Y=255;

          Z=Y;
          N=(byte)(Y&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0x8a:     //  TXA

          A=X;

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0x8c:     //  STY (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          bus.write_mem(M,(char)Y);
          if (count_cycles==1) cycles=4;
          break;

        case 0x8d:     //  STA (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          bus.write_mem(M,(char)A);
          if (count_cycles==1) cycles=4;
          break;

        case 0x8e:     //  STX (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          bus.write_mem(M,(char)X);
          if (count_cycles==1) cycles=4;
          break;

        case 0x90:     //  BCC
          branch=(byte)bus.read_mem(PC+1);

          if (C==0)
          {
            PC=PC+branch;
            if (count_cycles==1)
            {
              if ((PC&255)!=((PC-branch)&255))
              { cycles=4; }
              else
              { cycles=3; }
            }
          }
            else
          if (count_cycles==1) cycles=2;
          PC++;

          break;

        case 0x91:     //  STA ((Indirect), Y)

          M=bus.read_mem(++PC);
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8)+Y;
          bus.write_mem(M,(char)A);
          if (count_cycles==1) cycles=6;
          break;

        case 0x94:     //  STY (Zero Page, X)

          M=bus.read_mem(++PC)+X;
          bus.write_mem(M,(char)Y);
          if (count_cycles==1) cycles=4;
          break;

        case 0x95:     //  STA (Zero Page, X)

          M=bus.read_mem(++PC)+X;
          bus.write_mem(M,(char)A);
          if (count_cycles==1) cycles=4;
          break;

        case 0x96:     //  STX (Zero Page, Y)

          M=bus.read_mem(++PC)+Y;
          bus.write_mem(M,(char)X);
          if (count_cycles==1) cycles=3;
          break;

        case 0x98:     //  TYA

          A=Y;

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0x99:     //  STA (Absolute, Y)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+Y;
          bus.write_mem(M,(char)A);
          if (count_cycles==1) cycles=5;
          break;

        case 0x9a:     //  TXS

          SP=X;
          if (count_cycles==1) cycles=2;
          break;

        case 0x9d:     //  STA (Absolute, X)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+X;
          bus.write_mem(M,(char)A);
          if (count_cycles==1) cycles=5;
          break;

        case 0xa0:     //  LDY (Immediate)

          Y=bus.read_mem(++PC);

          Z=Y;
          N=(byte)(Y&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xa1:     //  LDA ((Indirect, X))

          M=bus.read_mem(++PC)+X;
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8);
          A=bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=6;
          break;

        case 0xa2:     //  LDX (Immediate)

          X=bus.read_mem(++PC);

          Z=X;
          N=(byte)(X&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xa4:     //  LDY (Zero Page)

          M=bus.read_mem(++PC);
          Y=bus.read_mem(M);

          Z=Y;
          N=(byte)(Y&128);
          if (count_cycles==1) cycles=3;
          break;

        case 0xa5:     //  LDA (Zero Page)

          M=bus.read_mem(++PC);
          A=bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=3;
          break;

        case 0xa6:     //  LDX (Zero Page)

          M=bus.read_mem(++PC);
          X=bus.read_mem(M);

          Z=X;
          N=(byte)(X&128);
          if (count_cycles==1) cycles=3;
          break;

        case 0xa8:     //  TAY

          Y=A;

          Z=Y;
          N=(byte)(Y&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xa9:     //  LDA (Immediate)

          A=bus.read_mem(++PC);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xaa:     //  TAX

          X=A;

          Z=X;
          N=(byte)(X&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xac:     //  LDY (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          Y=bus.read_mem(M);

          Z=Y;
          N=(byte)(Y&128);
          if (count_cycles==1) cycles=4;
          break;

        case 0xad:     //  LDA (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          A=bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=4;
          break;

        case 0xae:     //  LDX (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          X=bus.read_mem(M);

          Z=X;
          N=(byte)(X&128);
          if (count_cycles==1) cycles=4;
          break;

        case 0xb0:     //  BCS
          branch=(byte)bus.read_mem(PC+1);

          if (C!=0)
          {
            PC=PC+branch;
            if (count_cycles==1)
            {
              if ((PC&255)!=((PC-branch)&255))
              { cycles=3; }
              else
              { cycles=2; }
            }
          }
            else
          if (count_cycles==1) cycles=2;
          PC++;

          break;

        case 0xb1:     //  LDA ((Indirect), Y)

          M=bus.read_mem(++PC);
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8)+Y;
          A=bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=6; }
            else
            { cycles=5; }
          }
          break;

        case 0xb4:     //  LDY (Zero Page, X)

          M=bus.read_mem(++PC)+X;
          Y=bus.read_mem(M);

          Z=Y;
          N=(byte)(Y&128);
          if (count_cycles==1) cycles=4;
          break;

        case 0xb5:     //  LDA (Zero Page, X)

          M=bus.read_mem(++PC)+X;
          A=bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1) cycles=4;
          break;

    case 0xb6:     //  LDX (Zero Page, Y)

          M=bus.read_mem(++PC)+Y;
          X=bus.read_mem(M);

          Z=X;
          N=(byte)(X&128);
          if (count_cycles==1) cycles=4;
          break;

    case 0xb8:     //  CLV
      status.v = 0;
      return 2;

    case 0xb9:     //  LDA (Absolute, Y)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+Y;
          A=bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0xba:     //  TSX
          X=SP;

          Z=X;
          N=(byte)(X&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xbc:     //  LDY (Absolute, X)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+X;
          Y=bus.read_mem(M);

          Z=Y;
          N=(byte)(Y&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-X)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0xbd:     //  LDA (Absolute, X)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+X;
          A=bus.read_mem(M);

          Z=A;
          N=(byte)(A&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-X)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0xbe:     //  LDX (Absolute, Y)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+Y;
          X=bus.read_mem(M);

          Z=X;
          N=(byte)(X&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0xc0:     //  CPY (Immediate)

          temp=Y-bus.read_mem(++PC);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xc1:     //  CMP ((Indirect, X))

          M=bus.read_mem(++PC)+X;
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8);
          temp=A-bus.read_mem(M);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1) cycles=6;
          break;

        case 0xc4:     //  CPY (Zero Page)

          M=bus.read_mem(++PC);
          temp=Y-bus.read_mem(M);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1) cycles=3;
          break;

        case 0xc5:     //  CMP (Zero Page)

          M=bus.read_mem(++PC);
          temp=A-bus.read_mem(M);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1) cycles=3;
          break;

        case 0xc6:     //  DEC (Zero Page)

          M=bus.read_mem(++PC);
          if (bus.read_mem(M)==0)
          { bus.write_mem(M,(char)255); }
          else
          { bus.dec_mem(M); }

          Z=bus.read_mem(M);
          N=(byte)(bus.read_mem(M)&128);
          if (count_cycles==1) cycles=5;
          break;

        case 0xc8:     //  INY (Implied)
          Y++;
          if (Y>255) Y=0;

          Z=Y;
          N=(byte)(Y&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xc9:     //  CMP (Immediate)

          temp=A-bus.read_mem(++PC);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xca:     //  DEX
          X--;
          if (X<0) X=255;

          Z=X;
          N=(byte)(X&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xcc:     //  CPY (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          temp=Y-bus.read_mem(M);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1) cycles=4;
          break;

        case 0xcd:     //  CMP (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          temp=A-bus.read_mem(M);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1) cycles=4;
          break;

        case 0xce:     //  DEC (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          if (bus.read_mem(M)==0)
          { bus.write_mem(M,(char)255); }
          else
          { bus.dec_mem(M); }

          Z=bus.read_mem(M);
          N=(byte)(bus.read_mem(M)&128);
          if (count_cycles==1) cycles=6;
          break;

        case 0xd0:     //  BNE
          branch=(byte)bus.read_mem(PC+1);

          if (Z!=0)
          {
            PC=PC+branch;
            if (count_cycles==1)
            {
              if ((PC&255)!=((PC-branch)&255))
              { cycles=3; }
              else
              { cycles=2; }
            }
          }
            else
          if (count_cycles==1) cycles=2;
          PC++;

          break;

        case 0xd1:     //  CMP ((Indirect), Y)

          M=bus.read_mem(++PC);
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8)+Y;
          temp=A-bus.read_mem(M);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=6; }
            else
            { cycles=5; }
          }
          break;

        case 0xd5:     //  CMP (Zero Page, X)

          M=bus.read_mem(++PC)+X;
          temp=A-bus.read_mem(M);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1) cycles=4;
          break;

        case 0xd6:     //  DEC (Zero Page, X)

          M=bus.read_mem(++PC)+X;
          if (bus.read_mem(M)==0)
          { bus.write_mem(M,(char)255); }
          else
          { bus.dec_mem(M); }

          Z=bus.read_mem(M);
          N=(byte)(bus.read_mem(M)&128);
          if (count_cycles==1) cycles=6;
          break;

        case 0xd8:     //  CLD
          D=0;
          if (count_cycles==1) cycles=2;
          break;

        case 0xd9:     //  CMP (Absolute, Y)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+Y;
          temp=A-bus.read_mem(M);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0xdd:     //  CMP (Absolute, X)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+X;
          temp=A-bus.read_mem(M);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-X)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0xde:     //  DEC (Absolute, X)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+X;
          if (bus.read_mem(M)==0)
          { bus.write_mem(M,(char)255); }
          else
          { bus.dec_mem(M); }

          Z=bus.read_mem(M);
          N=(byte)(bus.read_mem(M)&128);
          if (count_cycles==1) cycles=7;
          break;

        case 0xe0:     //  CPX (Immediate)

          temp=X-bus.read_mem(++PC);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xe1:     //  SBC ((Indirect, X))

          M=bus.read_mem(++PC)+X;
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8);
          A=A-bus.read_mem(M)-(C^1);
          if (A<0)
          {
            C=1;
            A=A+256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1) cycles=6;
          break;

        case 0xe4:     //  CPX (Zero Page)

          M=bus.read_mem(++PC);
          temp=X-bus.read_mem(M);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1) cycles=3;
          break;

        case 0xe5:     //  SBC (Zero Page)
          M=bus.read_mem(++PC);
          A=A-bus.read_mem(M)-(C^1);
          if (A<0)
          {
            C=1;
            A=A+256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1) cycles=3;
          break;

        case 0xe6:     //  INC (Zero Page)

          M=bus.read_mem(++PC);
          bus.inc_mem(M);
          if (bus.read_mem(M)>255)
          { bus.write_mem(M,(char)0); }

          Z=bus.read_mem(M);
          N=(byte)(bus.read_mem(M)&128);
          if (count_cycles==1) cycles=5;
          break;

        case 0xe8:     //  INX (Implied)
          X++;
          if (X>255) X=0;

          Z=X;
          N=(byte)(X&128);
          if (count_cycles==1) cycles=2;
          break;

        case 0xe9:     //  SBC (Immediate)
          A=A-bus.read_mem(++PC)-(C^1);
          if (A<0)
          {
            C=1;
            A=A+256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1) cycles=2;
          break;

        case 0xea:     //  NOP
          if (count_cycles==1) cycles=2;
          break;

        case 0xec:     //  CPX (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          temp=X-bus.read_mem(M);
          if (temp<0)
          {
            temp=temp+256;
            C=1;
          }
          else
          { C=0; }

          Z=temp;
          N=(byte)(temp&128);
          if (count_cycles==1) cycles=4;
          break;

        case 0xed:     //  SBC (Absolute)
          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          A=A-bus.read_mem(M)-(C^1);
          if (A<0)
          {
            C=1;
            A=A+256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1) cycles=4;
          break;

        case 0xee:     //  INC (Absolute)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8);
          bus.inc_mem(M);
          if (bus.read_mem(M)>255)
          { bus.write_mem(M,(char)0); }

          Z=bus.read_mem(M);
          N=(byte)(bus.read_mem(M)&128);
          if (count_cycles==1) cycles=6;
          break;

        case 0xf0:     //  BEQ
          branch=(byte)bus.read_mem(PC+1);

          if (Z==0)
          {
            PC=PC+branch;
            if (count_cycles==1)
            {
              if ((PC&255)!=((PC-branch)&255))
              { cycles=3; }
              else
              { cycles=2; }
            }
          }
            else
          if (count_cycles==1) cycles=2;

          PC++;
          break;

        case 0xf1:     //  SBC ((Indirect), Y)

          M=bus.read_mem(++PC);
          M=bus.read_mem(M)+(bus.read_mem(M+1)<<8)+Y;
          A=A-bus.read_mem(M)-(C^1);
          if (A<0)
          {
            C=1;
            A=A+256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=6; }
            else
            { cycles=5; }
          }
          break;

        case 0xf5:     //  SBC (Zero Page, X)
          M=bus.read_mem(++PC)+X;
          A=A-bus.read_mem(M)-(C^1);
          if (A<0)
          {
            C=1;
            A=A+256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1) cycles=4;
          break;

        case 0xf6:     //  INC (Zero Page, X)

          M=bus.read_mem(++PC)+X;
          bus.inc_mem(M);
          if (bus.read_mem(M)>255)
          { bus.write_mem(M,(char)0); }

          Z=bus.read_mem(M);
          N=(byte)(bus.read_mem(M)&128);
          if (count_cycles==1) cycles=6;
          break;

        case 0xf8:     //  SED
          D=1;
          if (count_cycles==1) cycles=2;
          break;

        case 0xf9:     //  SBC (Absolute, Y)
          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+Y;
          A=A-bus.read_mem(M)-(C^1);
          if (A<0)
          {
            C=1;
            A=A+256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-Y)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0xfd:     //  SBC (Absolute, X)
          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+X;
          A=A-bus.read_mem(M)-(C^1);
          if (A<0)
          {
            C=1;
            A=A+256;
          }
          else
          { C=0; }

          Z=A;
          N=(byte)(A&128);
          V=(byte)(C^N);
          if (count_cycles==1)
          {
            if ((M&255)!=((M-X)&255))
            { cycles=5; }
            else
            { cycles=4; }
          }
          break;

        case 0xfe:     //  INC (Absolute, X)

          M=bus.read_mem(++PC);
          M=M+(bus.read_mem(++PC)<<8)+X;
          bus.inc_mem(M);
          if (bus.read_mem(M)>255)
          { bus.write_mem(M,(char)0); }

          Z=bus.read_mem(M);
          N=(byte)(bus.read_mem(M)&128);
          if (count_cycles==1) cycles=7;
          break;
#endif
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

