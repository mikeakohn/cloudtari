/**
 *  Cloudtari
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2021 by Michael Kohn
 *
 */

#ifndef RIOT_H
#define RIOT_H

#include <stdint.h>

class RIOT
{
public:
  RIOT();
  ~RIOT();

  void reset();
  uint8_t read_memory(int address);
  void write_memory(int address, uint8_t value);
  void clock(int ticks);
  void set_switch_reset()    { riot[SWCHB & 0x7] &= 0xfe; }
  void set_switch_select()   { riot[SWCHB & 0x7] &= 0xfd; }
  void clear_switch_reset()  { riot[SWCHB & 0x7] |= 0x01; }
  void clear_switch_select() { riot[SWCHB & 0x7] |= 0x02; }

  void clear_joystick_0_right() { riot[SWCHA & 0x7] |= 0x80; }
  void clear_joystick_0_left()  { riot[SWCHA & 0x7] |= 0x40; }
  void clear_joystick_0_down()  { riot[SWCHA & 0x7] |= 0x20; }
  void clear_joystick_0_up()    { riot[SWCHA & 0x7] |= 0x10; }
  void clear_joystick_1_right() { riot[SWCHA & 0x7] |= 0x08; }
  void clear_joystick_1_left()  { riot[SWCHA & 0x7] |= 0x04; }
  void clear_joystick_1_down()  { riot[SWCHA & 0x7] |= 0x02; }
  void clear_joystick_1_up()    { riot[SWCHA & 0x7] |= 0x01; }

  void set_joystick_0_right() { riot[SWCHA & 0x7] &= 0x7f; }
  void set_joystick_0_left()  { riot[SWCHA & 0x7] &= 0xbf; }
  void set_joystick_0_down()  { riot[SWCHA & 0x7] &= 0xdf; }
  void set_joystick_0_up()    { riot[SWCHA & 0x7] &= 0xef; }
  void set_joystick_1_right() { riot[SWCHA & 0x7] &= 0xf7; }
  void set_joystick_1_left()  { riot[SWCHA & 0x7] &= 0xfb; }
  void set_joystick_1_down()  { riot[SWCHA & 0x7] &= 0xfd; }
  void set_joystick_1_up()    { riot[SWCHA & 0x7] &= 0xfe; }

private:
  const int TIM1T = 1;
  const int TIM8T = 8;
  const int TIM64T = 64;
  const int T1024T = 1024;
  const int TIM1T_SHIFT = 0;
  const int TIM8T_SHIFT = 3;
  const int TIM64T_SHIFT = 6;
  const int T1024T_SHIFT = 10;

  const int SWCHA = 0x280;
  const int SWACNT = 0x281;
  const int SWCHB = 0x282;
  const int SWBCNT = 0x281;
  const int INTIM = 0x284;

  int prescale;
  int prescale_shift;
  int interrupt_timer = 255;

  uint8_t riot[8];
  uint8_t ram[256];
};

#endif

