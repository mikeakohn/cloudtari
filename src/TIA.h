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
  void build_player_0();
  void build_player_1();
  void clock();
  void clock(int ticks);
  void dump();
  bool wait_for_hsync() { return write_regs[WSYNC] != 0; }

  int compute_offset(int value)
  {
    int8_t offset = (int8_t)value;
    return offset >> 4;
  }

private:
  struct Playfield
  {
    Playfield() : data{0}, current_pixel{1} { }
    void reset() { current_pixel = 1; }
    bool is_pixel_on() { return (data & current_pixel) != 0; }

    bool is_pixel_on(int pos_x)
    {
      if (pos_x < 68) { return false; }
      pos_x = (pos_x - 68) / 4;

      return (data & (1ULL << pos_x)) != 0;
    }

    void next_pixel() { current_pixel <<= 1; }

    uint64_t data;
    uint64_t current_pixel;
  };

  struct Player
  {
    Player() : data{0}, set_pos{false}, scale{1}, start_pos{0}, offset{0} { }

    void reset() { start_pos = 0; }
    void set_position() { set_pos = true; }
    void set_position(int pos_x) { start_pos = pos_x; set_pos = false; }
    void set_scale(int value) { scale = value; }
    void set_offset(int value) { next_offset = value; }
    void apply_offset() { offset = next_offset; }
    void clear_offset() { offset = 0; next_offset = 0; }
    bool need_set_position() { return set_pos; }

    bool is_pixel_on(int pos_x)
    {
      int x = pos_x + offset;
      if (x < 68) { return false; }
      x = (x - start_pos) / scale;
      if (x >= 7) { return false; }
      return (data & (1 << x)) != 0;
    }

    uint8_t data;
    bool set_pos;
    int scale, start_pos, offset, next_offset;
  };

  int get_x() { return pos_x - 68; }
  int get_y() { return pos_y - 37; }
  void player_size(Player &player, int value);
  bool draw_playfield_fg();
  bool draw_player_0();
  bool draw_player_1();
  bool draw_missile_0();
  bool draw_missile_1();
  bool draw_ball();
  void draw_playfield_bg();
  void draw_pixel();

  struct Colors
  {
    uint32_t playfield;
    uint32_t background;
    uint32_t player_0;
    uint32_t player_1;
  } colors;

  Television *television;

  int pos_x;
  int pos_y;

  Playfield playfield;
  Player player_0;
  Player player_1;

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

