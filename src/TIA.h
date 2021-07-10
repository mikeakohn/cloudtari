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

#ifndef TIA_H
#define TIA_H

#include <stdint.h>

#include "ColorTable.h"
#include "Television.h"

class TIA
{
public:
  TIA();
  ~TIA();

  void reset();
  uint8_t read_memory(int address);
  void write_memory(int address, uint8_t value);
  void clock();
  void clock(int ticks);
  void dump();
  bool wait_for_hsync() { return write_regs[WSYNC] != 0; }
  void set_joystick_0_fire() { read_regs[INPT4] &= 0x7f; }
  void set_joystick_1_fire() { read_regs[INPT5] &= 0x7f; }
  void clear_joystick_0_fire() { read_regs[INPT4] |= 0x80; }
  void clear_joystick_1_fire() { read_regs[INPT5] |= 0x80; }

  bool need_check_events()
  {
    bool value = check_events;
    check_events = false;
    return value;
  }

  void set_image()
  {
    if (bitsize == 8)
    {
      image_8 = (uint8_t *)television->get_image();
    }
      else
    {
      image_32 = (uint32_t *)television->get_image();
    }
  }

  void set_television(Television *television)
  {
    this->television = television;
    bitsize = television->get_bitsize();
    set_image();
  }

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
    bool is_pixel_on() { return pixel_value; }

    //bool compute_pixel() { pixel_value = (data & current_pixel) != 0; }

    void compute_pixel(int pos_x)
    {
      //if (pos_x < 68) { return false; }
      pos_x = (pos_x - 68) / 4;

      pixel_value = (data & (1ULL << pos_x)) != 0;
    }

    void next_pixel() { current_pixel <<= 1; }

    bool pixel_value;
    uint64_t data;
    uint64_t current_pixel;
  };

  struct Player
  {
    Player() :
      data{0},
      set_pos{false},
      vertical_delay{false},
      need_update{false},
      scale{1},
      start_pos{0},
      offset{0},
      next_offset{0}
    {
    }

    void reset() { start_pos = 0; }
    void set_position() { set_pos = true; }
    void set_position(int pos_x) { start_pos = pos_x; set_pos = false; }
    void set_scale(int value) { scale = value; }
    void set_offset(int value) { next_offset = value; }
    void apply_offset() { offset = next_offset; }
    void clear_offset() { offset = 0; next_offset = 0; }
    bool need_set_position() { return set_pos; }
    bool is_pixel_on() { return pixel_value; }

    void compute_pixel(int pos_x)
    {
      //if (pos_x < 68) { return false; }
      if (pos_x < start_pos - offset) { pixel_value = false; return; }
      int x = (pos_x - (start_pos - offset)) / scale;
      if (x > 7) { pixel_value = false; return; }
      pixel_value = (data & (1 << x)) != 0;
    }

    uint8_t data;
    bool set_pos;
    bool vertical_delay;
    bool need_update;
    bool pixel_value;
    int scale, start_pos, offset, next_offset;
  };

  struct Sprite
  {
    Sprite() :
      width{1},
      set_pos{false},
      enabled{false},
      start_pos{0},
      offset{0}
    {
    }

    void reset() { start_pos = 0; }
    void set_position() { set_pos = true; }
    void set_position(int pos_x) { start_pos = pos_x; set_pos = false; }
    void set_width(int value) { width = value; }
    void set_offset(int value) { next_offset = value; }
    void apply_offset() { offset = next_offset; }
    void clear_offset() { offset = 0; next_offset = 0; }
    void set_enabled(bool value) { enabled = value; }
    bool need_set_position() { return set_pos; }
    bool is_pixel_on() { return pixel_value; }

    void compute_pixel(int pos_x)
    {
      if (!enabled) { pixel_value = false; return; }
      //if (pos_x < 68) { return false; }
      int x = start_pos - offset;
      pixel_value = pos_x >= x && pos_x < x + width;
    }

    uint8_t width;
    bool set_pos;
    bool enabled;
    bool pixel_value;
    int start_pos, offset, next_offset;
  };

  struct Missile : public Sprite
  {
  };

  struct Ball : public Sprite
  {
    Ball() : need_update{false}, vertical_delay{false} { };
    bool need_update;
    bool vertical_delay;
  };

  int get_x() { return pos_x - 68; }
  int get_y() { return pos_y - 40; }
  void player_size(Player &player, int value);
  void build_playfield();
  void build_player_0();
  void build_player_1();
  void draw_pixel();
  void compute_collisions();

  inline void set_pixel(int x, int y, uint8_t color)
  {
    x = x * 3;
    y = y * 2;

    const int width = television->get_width();
    int ptr = y * width + x;

    if (bitsize == 8)
    {
      color = color >> 1;
      image_8[ptr + 0] = color;
      image_8[ptr + 1] = color;
      image_8[ptr + 2] = color;
      ptr += width;
      image_8[ptr + 0] = color;
      image_8[ptr + 1] = color;
      image_8[ptr + 2] = color;
    }
      else
    {
      int c = ColorTable::get_color(color);
      image_32[ptr + 0] = c;
      image_32[ptr + 1] = c;
      image_32[ptr + 2] = c;
      ptr += width;
      image_32[ptr + 0] = c;
      image_32[ptr + 1] = c;
      image_32[ptr + 2] = c;
    }
  }

#if 0
  struct Colors
  {
    uint8_t playfield;
    uint8_t background;
    uint8_t player_0;
    uint8_t player_1;
  } colors;
#endif

  Television *television;

  int pos_x;
  int pos_y;
  bool hsync_latch;

  Playfield playfield;
  Player player_0;
  Player player_1;
  Missile missile_0;
  Missile missile_1;
  Ball ball;

  uint8_t write_regs[64];
  uint8_t read_regs[16];
  uint8_t reverse[256];

  uint32_t *image_32;
  uint8_t *image_8;
  int bitsize;
  bool check_events;

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

