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

M6502::M6502()
{
  reset();
}

M6502::~M6502()
{
}

M6502::reset()
{
  reg_a = 0;
  reg_x = 0;
  reg_y = 0;

  pc = 0;
  sp = 0xff;
  total_cycles = 0;
  total_instructions = 0;
}

M6502::dump()
{
  printf("----- 6502 -----\n");
  printf(" PC=0x%04x SP=0x%04x\n", pc, sp);
  printf(" A=%d X=%d Y=%d\n", reg_a, reg_x, reg_y);
  printf(" N V B * D I Z C\n");
  printf(" %d %d %d %d %d %d %d %d\n",
    reg_p.n,
    reg_p.v,
    reg_p.b,
    reg_p.one,
    reg_p.d,
    reg_p.i,
    reg_p.z,
    reg_p.c);
  printf(" total_instructions=%d\n", total_instructions);
  printf(" total_cycles=%d\n", total_cycles);
}

