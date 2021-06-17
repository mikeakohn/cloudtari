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

#include "Television.h"

class TIA
{
public:
  TIA();
  ~TIA();

  void init();
  void set_television(Television *television) { this->television = television; }
  uint8_t read_memory(int address);
  void write_memory(int address, uint8_t value);
  void build_playfield();
  void clock();
  void clock(int ticks);
  bool draw_playfield_fg();
  bool draw_player_0();
  bool draw_player_1();
  bool draw_missile_0();
  bool draw_missile_1();
  bool draw_ball();
  void draw_playfield_bg();
  void draw_pixel();
  bool wait_for_hsync() { return write_regs[WSYNC] != 0; }

private:
  Television *television;

  int pos_x;
  int pos_y;
  int pf_pixel;
  uint64_t playfield;

  uint8_t write_regs[64];
  uint8_t read_regs[16];
  uint8_t reverse[256];

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
    RESBL = 0x14,
    AUDC0 = 0x15,
    AUDC1 = 0x16,
    AUDF0 = 0x17,
    AUDF1 = 0x18,
    AUDV0 = 0x19,
    AUDV1 = 0x1a,
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

