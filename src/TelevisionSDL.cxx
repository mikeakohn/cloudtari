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

#include "TelevisionSDL.h"

TelevisionSDL::TelevisionSDL()
{
  SDL_Init(SDL_INIT_EVERYTHING);

  //window = DL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
  window = SDL_CreateWindow(
    "Debug",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    640,
    480,
    0);
}

TelevisionSDL::~TelevisionSDL()
{
  //if (screen != NULL) { SDL_FreeSurface(screen); }
  if (window != NULL) { SDL_DestroyWindow(window); }

  SDL_Quit();
}

int TelevisionSDL::init()
{
  return 0;
}

void TelevisionSDL::clear_display()
{
}

void TelevisionSDL::draw_pixel(int x, int y, uint32_t color)
{
}

bool TelevisionSDL::refresh()
{
  return true;
}

int TelevisionSDL::handle_events()
{
  SDL_Event event;

  if (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
          return -1;
        }

        break;
      case SDL_QUIT:
        return -1;
        break;
  }

  return 0;
}

  return 0;
}

