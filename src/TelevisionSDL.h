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

#ifndef TELEVISION_SDL_H
#define TELEVISION_SDL_H

#include <stdint.h>
#include <SDL2/SDL.h>

#include "Television.h"

class TelevisionSDL : public Television
{
public:
  TelevisionSDL();
  virtual ~TelevisionSDL();

  virtual int init();
  virtual void clear_display();
  virtual void draw_pixel(int x, int y, uint32_t color);
  virtual void draw_pixel(int x, int y, uint8_t color);
  virtual bool refresh();
  virtual int handle_events();

private:
  SDL_Surface *screen;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Rect background_rect;
  Uint32 background_color;

};

#endif

