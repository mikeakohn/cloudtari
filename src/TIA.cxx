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

#include <string.h>

#include "TIA.h"

// Atari 2600 has:
// 262 horizontal lines (37 vertical blank, 192 picture, 30 overscan)
// Horizontal scanline is: 68 hsync, 160 pixels.
// Total Resolution: 228x262
// Visible Resolution: 160x192

TIA::TIA() :
  pos_x(0),
  pos_y(0)
{
  for (int n = 0; n < 256; n++)
  {
    reverse[n]=
      (((n & 0x01) << 7) | ((n & 0x02) << 5) |
       ((n & 0x04) << 3) | ((n & 0x08) << 1) |
       ((n & 0x10) >> 1) | ((n & 0x20) >> 3) |
       ((n & 0x40) >> 5) | ((n & 0x80) >> 7));
  }
}

TIA::~TIA()
{
}

void TIA::init()
{
#if 0
  television.init();
#endif

  pos_x = 0;
  pf_pixel = 1;

  memset(write_regs, 0, sizeof(write_regs));
  memset(read_regs, 0, sizeof(read_regs));
}

uint8_t TIA::read_memory(int address)
{
  if (address > 0x0d) { return 0; }

  return read_regs[address];
}

void TIA::write_memory(int address, uint8_t value)
{
  if (address > 0x2c) { return; }

  switch (address)
  {
    case WSYNC:
      write_regs[WSYNC] = 1;
      break;
    case VSYNC:
      television->refresh();
      pos_x = 0;
      pos_y = 0;
      break;
    case PF0:
    case PF1:
    case PF2:
      break;
    case RESP0:
      // Reset player 0.
      if ((write_regs[REFP0] & 8) == 0)
      {
        //player0 = 0x800 | reverse[write_regs[GRP0]];
      }
        else
      {
        //player0 = 0x800 | write_regs[GRP0];
      }

      //player0_clocks = 0;

      break;
    case RESP1:
      // Reset player 1.
      break;
    case RESM0:
      // Reset missile 0.
      break;
    case RESM1:
      // Reset missile 1.
      break;
    case RESBL:
      // Reset ball.
      break;
    case HMCLR:
      write_regs[HMP0] = 0;
      write_regs[HMP1] = 0;
      write_regs[HMM0] = 0;
      write_regs[HMM1] = 0;
      write_regs[HMBL] = 0;
      break;
    default:
      write_regs[address] = value;
  }
}

void TIA::build_playfield()
{
  playfield =
    ((write_regs[PF0] >> 4) & 0x0f) |
     (reverse[write_regs[PF1]] << 4) |
     (write_regs[PF2] << 12);

  if ((write_regs[CTRLPF] & 1) == 0)
  {
    // Non-mirroed playfield
    playfield |= playfield << 20;
  }
    else
  {
    // Mirrored playfield.
    playfield |=
      (((uint64_t)reverse[write_regs[PF0]] & 0x0f) << 36) |
       ((uint64_t)write_regs[PF1] << 28) |
       (reverse[write_regs[PF2]] << 20);
  }
}

void TIA::clock()
{
  // The main playfield area starts after 68 clocks.
  if (pos_x >= 68)
  {
    // The pf_pixel points to the next bit to be displayed from PF0, PF1, PF2.
    if (pos_x == 68) { pf_pixel = 1; }

    draw_pixel();

    // Every 4 screen pixels, increment playfield pixel.
    if (((pos_x - 68) % 4) == 0) { pf_pixel = pf_pixel << 1; }

#if 0
    if ((player0 & 0xf00) != 0)
    {
      player0_clocks++;

      if ((player0_clocks % (1 << (write_regs[NUSIZ0] & 0x03))) == 0)
      {
        player0 = player0 >> 1;
      }
    }
#endif

  }

  pos_x++;

  if (pos_x == 68 + 160)
  {
    write_regs[WSYNC] = 0;
    pos_x = 0;
    pos_y++;
  }
}

void TIA::clock(int ticks)
{
  // Every CPU cycle is 3 pixels.
  ticks = ticks * 3;

  //while (ticks > 0 || write_regs[WSYNC] != 0)

  while (ticks > 0)
  {
    clock();
    ticks--;
  }
}

bool TIA::draw_playfield_fg()
{
  if ((playfield & pf_pixel) != 0)
  { 
    television->draw_pixel(pos_x, pos_y, write_regs[COLUPF]);

    return true;
  }

  return false;
}

bool TIA::draw_player_0()
{
  return false;
}

bool TIA::draw_player_1()
{
  return false;
}

bool TIA::draw_missile_0()
{
  return false;
}

bool TIA::draw_missile_1()
{
  return false;
}

bool TIA::draw_ball()
{
  return false;
}

void TIA::draw_playfield_bg()
{
  if ((playfield & pf_pixel) == 0)
  {
    television->draw_pixel(pos_x, pos_y, write_regs[COLUBK]);
  }
}

void TIA::draw_pixel()
{
  if ((write_regs[CTRLPF] & 4) == 0)
  {
    // Sprites have priority.
    if (draw_player_0()) { return; }
    if (draw_missile_0()) { return; }
    if (draw_player_1()) { return; }
    if (draw_missile_1()) { return; }
    if (draw_playfield_fg()) { return; }
    if (draw_ball()) { return; }
  }
    else
  {
    // Playfield has priority.
    if (draw_playfield_fg()) { return; }
    if (draw_ball()) { return; }
    if (draw_player_0()) { return; }
    if (draw_missile_0()) { return; }
    if (draw_player_1()) { return; }
    if (draw_missile_1()) { return; }
  }

  draw_playfield_bg();
}

