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

#ifndef TIA_H
#define TIA_H

#include <stdint.h>

class TIA
{
public:
  TIA();
  ~TIA();

  void init();
  uint8_t read_memory(int address);
  void write_memory(int address, uint8_t value);
  void clock();
  void clock(int ticks);
  bool draw_playfield_fg();
  bool draw_player1();
  bool draw_player2();
  bool draw_ball();
  void draw_playfield_bg();
  void draw_pixel();

private:
  int pos_x;
  int pos_y;
  bool wait_for_hsync;
  int pf_pixel;

  uint8_t write_regs[128];
  uint8_t read_regs[128];

  enum WriteAddress
  {
    VSYNC = 0x00,
    VBLANK = 0x01,
    WSYNC = 0x02,
    RSYNC = 0x03,
    NUSIZ0 = 0x04,
    NUSIZ1 = 0x05,
    COLUP0 = 0x06,
    COLUP1 = 0x07,
    COLUPF = 0x08,
    COLUBK = 0x09,
    CTRLPF = 0x0a,
    REFP0 = 0x0b,
    REFP1 = 0x0c,
    PF0 = 0x0d,
    PF1 = 0x0e,
    PF2 = 0x0f,
    RESP0 = 0x10,
    RESP1 = 0x11,
    RESM0 =  0x12,
    RESM1 = 0x13,
    GRP0 = 0x1b,
    GRP1 = 0x1c,
    ENAM0 = 0x1d,
    ENAM1 =  0x1e,
    ENABL = 0x1f,
    HMP0 = 0x20,
    HMP1 = 0x21,
    HMM0 = 0x22,
    HMM1 = 0x23,
    HMBL = 0x24,
    VDELP0 = 0x25,
    VDELP1 = 0x26,
    VDELBL = 0x27,
    RESMP0 = 0x28,
    RESMP1 = 0x29,
    HMOVE = 0x2a,
    HMCLR = 0x2b,
    CXCLR = 0x2c,
  };

  enum ReadAddress
  {
    CXM0P = 0x00,
    CXM1P = 0x01,
    CXP0FB = 0x02,
    CXP1FB = 0x03,
    CXM0FB = 0x04,
    CXM1FB = 0x05,
    CXBLPF = 0x06,
    CXPPMM = 0x07,
    INPT4 = 0x0c,
    INPT5 = 0x0d,
  };
};

#endif

