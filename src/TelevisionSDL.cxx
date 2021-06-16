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

  background_rect.x = 0;
  background_rect.y = 0;
  background_rect.w = 640;
  background_rect.h = 480;

  //renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

  //SDL_RenderClear(renderer);
  screen = SDL_GetWindowSurface(window);

  background_color = SDL_MapRGB(screen->format, 50, 50, 100);

  clear_display();
}

TelevisionSDL::~TelevisionSDL()
{
  if (screen != NULL) { SDL_FreeSurface(screen); }
  if (window != NULL) { SDL_DestroyWindow(window); }

  SDL_Quit();
}

int TelevisionSDL::init()
{
  return 0;
}

void TelevisionSDL::clear_display()
{
  //SDL_RenderClear(renderer);
  SDL_FillRect(screen, &background_rect, background_color);
  SDL_UpdateWindowSurface(window);
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
    }
  }

  return 0;
}

