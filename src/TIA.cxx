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

#include "TIA.h"

// Atari 2600 has:
// 262 horizontal lines (37 vertical blank, 192 picture, 30 overscan)
// Horizontal scanline is: 68 hsync, 160 pixels.

TIA::TIA() :
  pos_x(0),
  pos_y(0),
  wait_for_hsync(false)
{
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
  wait_for_hsync = false;
  pf_pixel = 1;
}

uint8_t TIA::read_memory(int address)
{
  return 0;
}

void TIA::write_memory(int address, uint8_t value)
{
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
    wait_for_hsync = false;
    pos_x = 0;
    pos_y++;
  }
}

void TIA::clock(int ticks)
{
  ticks = ticks * 3;

  while (ticks > 0 || wait_for_hsync)
  {
    clock();
    ticks--;
  }
}

bool TIA::draw_playfield_fg()
{
  return false;
}

bool TIA::draw_player1()
{
  return false;
}

bool TIA::draw_player2()
{
  return false;
}

bool TIA::draw_ball()
{
  return false;
}

void TIA::draw_playfield_bg()
{
}

void TIA::draw_pixel()
{
  if ((write_regs[CTRLPF] & 4) == 0)
  {
    // Sprites have priority.
    if (draw_player1()) { return; }
    if (draw_player2()) { return; }
    if (draw_ball()) { return; }
    if (draw_playfield_fg()) { return; }
  }
    else
  {
    // Playfield has priority.
    if (draw_playfield_fg()) { return; }
    if (draw_player1()) { return; }
    if (draw_player2()) { return; }
    if (draw_ball()) { return; }
  }

  draw_playfield_bg();
}

