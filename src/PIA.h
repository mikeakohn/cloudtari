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

#ifndef PIA_H
#define PIA_H

#include <stdint.h>

class PIA
{
public:
  PIA();
  ~PIA();

  void reset();
  uint8_t read_memory(int address);
  void write_memory(int address, uint8_t value);

private:
  const int TIM1T = 1;
  const int TIM8T = 8;
  const int TIM64T = 64;
  const int T1024T = 1024;
  const int TIM1T_S = 0;
  const int TIM8T_S = 3;
  const int TIM64T_S = 6;
  const int T1024T_S = 10;

  int prescale;
  int prescale_s;
  int interrupt_timer = 255;

  uint8_t riot[8];
  uint8_t ram[256];
};

#endif
