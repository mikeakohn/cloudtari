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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ColorTable.h"
#include "TIA.h"

// Atari 2600 has:
// 262 horizontal lines (37 vertical blank, 192 picture, 30 overscan)
// Horizontal scanline is: 68 hsync, 160 pixels.
// Total Resolution: 228x262
// Visible Resolution: 160x192

TIA::TIA() :
  pos_x{0},
  pos_y{0},
  hsync_latch{false},
  image_32{nullptr},
  image_8{nullptr},
  check_events{false},
  fps{0},
  timestamp{0}
{
  for (int n = 0; n < 256; n++)
  {
    reverse[n]=
      (((n & 0x01) << 7) | ((n & 0x02) << 5) |
       ((n & 0x04) << 3) | ((n & 0x08) << 1) |
       ((n & 0x10) >> 1) | ((n & 0x20) >> 3) |
       ((n & 0x40) >> 5) | ((n & 0x80) >> 7));
  }

  reset();
}

TIA::~TIA()
{
}

void TIA::reset()
{
  pos_x = 0;
  pos_y = 0;
  hsync_latch = false;

  playfield.reset();

  memset(write_regs, 0, sizeof(write_regs));
  memset(read_regs, 0, sizeof(read_regs));

  read_regs[INPT4] = 0x80;
  read_regs[INPT5] = 0x80;
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
    case VSYNC:
      if ((value & 2) == 2)
      {
        //show_fps();

        television->refresh();
        pos_x = 0;
        pos_y = 0;

        // In case the Television is page flipping.
        set_image();
      }

      write_regs[VSYNC] = value;
      break;
    case VBLANK:
      break;
    case WSYNC:
      //write_regs[WSYNC] = 1;
      hsync_latch = true;
      break;
    case RSYNC:
      // "This address resets the horizontal sync counter to define the
      // beginning of horizontal blank time, and is used in chip testing."
      pos_x = 0;
      break;
    case NUSIZ0:
      player_size(player_0, value);
      missile_0.set_width(1 << ((value >> 4) & 0x3));
      break;
    case NUSIZ1:
      player_size(player_1, value);
      missile_1.set_width(1 << ((value >> 4) & 0x3));
      break;
#if 0
    case COLUP0:
      colors.player_0 = value;
      break;
    case COLUP1:
      colors.player_1 = value;
      break;
    case COLUBK:
      colors.background = value;
      break;
    case COLUPF:
      colors.playfield = value;
      break;
#endif
    case CTRLPF:
      write_regs[CTRLPF] = value;
      ball.set_width(1 << ((value >> 4) & 0x3));
      break;
    case REFP0:
      write_regs[REFP0] = value;
      build_player_0();
      break;
    case REFP1:
      write_regs[REFP1] = value;
      build_player_1();
      break;
    case PF0:
    case PF1:
    case PF2:
      write_regs[address] = value;
      build_playfield();
      break;
    case RESP0:
      // Reset player 0 (aka, start drawing).
      player_0.set_position();
      break;
    case RESP1:
      // Reset player 1 (aka, start drawing).
      player_1.set_position();
      break;
    case RESM0:
      // Reset missile 0.
      missile_0.set_position();
      break;
    case RESM1:
      // Reset missile 1.
      missile_1.set_position();
      break;
    case RESBL:
      // Reset ball.
      ball.set_position();
      break;
    case AUDC0:
    case AUDC1:
    case AUDF0:
      break;
    case AUDF1:
      break;
    case AUDV0:
    case AUDV1:
      break;
    case GRP0:
      write_regs[GRP0] = value;

      if (!player_0.vertical_delay)
      {
        build_player_0();
      }
        else
      {
        player_0.need_update = true;
      }
      break;
    case GRP1:
      write_regs[GRP1] = value;

      if (!player_1.vertical_delay)
      {
        build_player_1();
      }
        else
      {
        player_1.need_update = true;
      }
      break;
    case ENAM0:
      missile_0.set_enabled((value & 2) != 0);
      break;
    case ENAM1:
      missile_1.set_enabled((value & 2) != 0);
      break;
    case ENABL:
      if ((value & 2) == 0)
      {
        ball.set_enabled(false);
      }
        else
      {
        if (!ball.vertical_delay)
        {
          ball.set_enabled(true);
        }
          else
        {
          ball.need_update = true;
        }
      }
      break;
    case HMP0:
      player_0.set_offset(compute_offset(value));
      break;
    case HMP1:
      player_1.set_offset(compute_offset(value));
      break;
    case HMM0:
      missile_0.set_offset(compute_offset(value));
      break;
    case HMM1:
      missile_1.set_offset(compute_offset(value));
      break;
    case HMBL:
      ball.set_offset(compute_offset(value));
      break;
    case VDELP0:
      player_0.vertical_delay = value & 1;
      break;
    case VDELP1:
      player_1.vertical_delay = value & 1;
      break;
    case VDELBL:
      ball.vertical_delay = value & 1;
      break;
    case HMOVE:
      player_0.apply_offset();
      player_1.apply_offset();
      missile_0.apply_offset();
      missile_1.apply_offset();
      ball.apply_offset();
      break;
    case HMCLR:
      // Clear motion registers.
      player_0.clear_offset();
      player_1.clear_offset();
      missile_0.clear_offset();
      missile_1.clear_offset();
      ball.clear_offset();
      break;
    case CXCLR:
      // Clear collision latches.
      read_regs[CXM0P] = 0;
      read_regs[CXM1P] = 0;
      read_regs[CXP0FB] = 0;
      read_regs[CXP1FB] = 0;
      read_regs[CXM0FB] = 0;
      read_regs[CXM1FB] = 0;
      read_regs[CXBLPF] = 0;
      read_regs[CXPPMM] = 0;
      break;
    default:
      write_regs[address] = value;
      break;
  }
}

void TIA::clock()
{
  // The main playfield area starts after 68 clocks.
  if (pos_x >= 68 && pos_y >= 40 && pos_y < 232)
  {
    // The point to the first bit to be displayed from PF0, PF1, PF2.
    if (pos_x == 68) { playfield.reset(); }

    playfield.compute_pixel(pos_x);
    player_0.compute_pixel(pos_x);
    player_1.compute_pixel(pos_x);
    missile_0.compute_pixel(pos_x);
    missile_1.compute_pixel(pos_x);
    ball.compute_pixel(pos_x);

    draw_pixel();
    compute_collisions();
  }

  pos_x++;

  if (pos_x == 68 + 160)
  {
    if (player_0.need_update) { build_player_0(); }
    if (player_1.need_update) { build_player_1(); }

    if (!hsync_latch) { write_regs[WSYNC] = 0; }

#if 0
    if (pos_y >= 40 && pos_y <= 232)
    {
      television->refresh();
    }
#endif

    if (pos_y < 40) { check_events = true; }

    pos_x = 0;
    pos_y++;

    // This really shouldn't be needed.. but..
    if (pos_y > 262) { pos_y = 0; }
  }
}

void TIA::clock(int ticks)
{
  // Every CPU cycle is 3 pixels.
  ticks = ticks * 3;

  while (ticks > 0)
  {
    clock();
    ticks--;
  }

  // For some reason (according to some forum) the TIA delays setting the
  // X position for player sprites by 5 TIA clocks and ball / missile by 4.
  if (player_0.need_set_position())  { player_0.set_position(pos_x + 5); }
  if (player_1.need_set_position())  { player_1.set_position(pos_x + 5); }
  if (missile_0.need_set_position()) { missile_0.set_position(pos_x + 4); }
  if (missile_1.need_set_position()) { missile_1.set_position(pos_x + 4); }
  if (ball.need_set_position())      { ball.set_position(pos_x + 4); }

  if (hsync_latch)
  {
    write_regs[WSYNC] = 1;
    hsync_latch = false;
  }
}

void TIA::dump()
{
  printf("TIA: pos_x=%d pos_y=%d  wait_for_hsync=%s\n",
    pos_x, pos_y, wait_for_hsync() ? "on" : "off");

  printf("playfield: ");
  for (int n = 0; n < 40; n++)
  {
    printf("%c", (playfield.data & (1ULL << n)) != 0 ? '*' : '.');
  }
  printf("\n");

  printf("player_0: ");
  for (int n = 0; n < 8; n++)
  {
    printf("%c", (player_0.data & (1 << n)) != 0 ? '*' : '.');
  }
  printf(" x=%d offset=%d (next=%d)\n",
    player_0.start_pos,
    player_0.offset,
    player_0.next_offset);

  printf("player_1: ");
  for (int n = 0; n < 8; n++)
  {
    printf("%c", (player_1.data & (1 << n)) != 0 ? '*' : '.');
  }
  printf(" x=%d offset=%d (next=%d)\n",
    player_1.start_pos,
    player_1.offset,
    player_1.next_offset);
}

void TIA::player_size(Player &player, int value)
{
  int temp = value & 7;

  player.set_scale(1);

  switch (temp)
  {
    case 0:
      // One copy.
      break;
    case 1:
      // Two copies close.
      break;
    case 2:
      // Two copies med.
      break;
    case 3:
      // Three copies close.
      break;
    case 4:
      // Two copies wide.
      break;
    case 5:
      // Double size player.
      player.set_scale(2);
      break;
    case 6:
      // 3 copies medium.
      break;
    case 7:
      // Quad size player.
      player.set_scale(4);
      break;
  }
}

void TIA::build_playfield()
{
  playfield.data =
    ((write_regs[PF0] >> 4) & 0x0f) |
     (reverse[write_regs[PF1]] << 4) |
     (write_regs[PF2] << 12);

  if ((write_regs[CTRLPF] & 1) == 0)
  {
    // Non-mirroed playfield
    playfield.data |= playfield.data << 20;
  }
    else
  {
    // Mirrored playfield.
    playfield.data |=
      (((uint64_t)reverse[write_regs[PF0]] & 0x0f) << 36) |
       ((uint64_t)write_regs[PF1] << 28) |
       ((uint64_t)reverse[write_regs[PF2]] << 20);
  }
}

void TIA::build_player_0()
{
  if ((write_regs[REFP0] & 0x08) == 0)
  {
    player_0.data = reverse[write_regs[GRP0]];
  }
    else
  {
    player_0.data = write_regs[GRP0];
  }

  player_0.need_update = false;
}

void TIA::build_player_1()
{
  if ((write_regs[REFP1] & 0x08) == 0)
  {
    player_1.data = reverse[write_regs[GRP1]];
  }
    else
  {
    player_1.data = write_regs[GRP1];
  }

  player_1.need_update = false;
}

void TIA::draw_pixel()
{
  uint8_t color = write_regs[COLUBK];

  if ((write_regs[CTRLPF] & 4) == 0)
  {
    // Sprites have priority.
    do
    {
      if (player_0.is_pixel_on())  { color = write_regs[COLUP0]; break; }
      if (missile_0.is_pixel_on()) { color = write_regs[COLUP0]; break; }
      if (player_1.is_pixel_on())  { color = write_regs[COLUP1]; break; }
      if (missile_1.is_pixel_on()) { color = write_regs[COLUP1]; break; }
      if (playfield.is_pixel_on()) { color = write_regs[COLUPF]; break; }
      if (ball.is_pixel_on())      { color = write_regs[COLUPF]; break; }
    } while (false);
  }
    else
  {
    // Playfield has priority.
    do
    {
      if (playfield.is_pixel_on()) { color = write_regs[COLUPF]; break; }
      if (ball.is_pixel_on())      { color = write_regs[COLUPF]; break; }
      if (player_0.is_pixel_on())  { color = write_regs[COLUP0]; break; }
      if (missile_0.is_pixel_on()) { color = write_regs[COLUP0]; break; }
      if (player_1.is_pixel_on())  { color = write_regs[COLUP1]; break; }
      if (missile_1.is_pixel_on()) { color = write_regs[COLUP1]; break; }
    } while (false);
  }

  set_pixel(get_x(), get_y(), color);
}

void TIA::compute_collisions()
{
  const int m0 = missile_0.is_pixel_on();
  const int m1 = missile_1.is_pixel_on();
  const int p0 = player_0.is_pixel_on();
  const int p1 = player_1.is_pixel_on();
  const int bl = ball.is_pixel_on();
  const int pf = playfield.is_pixel_on();

  read_regs[CXM0P] |= ((m0 & p1) << 7) | ((m0 & p0) << 6);
  read_regs[CXM1P] |= ((m1 & p0) << 7) | ((m1 & p1) << 6);
  read_regs[CXP0FB] |= ((p0 & pf) << 7) | ((p0 & bl) << 6);
  read_regs[CXP1FB] |= ((p1 & pf) << 7) | ((p1 & bl) << 6);
  read_regs[CXM0FB] |= ((m0 & pf) << 7) | ((m0 & bl) << 6);
  read_regs[CXM1FB] |= ((m1 & pf) << 7) | ((m1 & bl) << 6);
  read_regs[CXBLPF] |= ((bl & pf) << 7);
  read_regs[CXPPMM] |= ((p0 & p1) << 7) | ((m0 & m1) << 6);
}

