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

#include "ColorTable.h"
#include "TelevisionSDL.h"

TelevisionSDL::TelevisionSDL()
{
  SDL_Init(SDL_INIT_EVERYTHING);

  //window = DL_SetVideoMode(640, width, 32, SDL_SWSURFACE);
  window = SDL_CreateWindow(
    "Debug",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    width,
    height,
    0);

  background_rect.x = 0;
  background_rect.y = 0;
  background_rect.w = width;
  background_rect.h = height;

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

void TelevisionSDL::draw_pixel(int x, int y, uint8_t color)
{
  SDL_Rect rect;

  rect.x = x * 3;
  rect.y = y * 2;
  rect.w = 3;
  rect.h = 2;

  SDL_FillRect(screen, &rect, ColorTable::get_color(color));
}

bool TelevisionSDL::refresh()
{
  SDL_UpdateWindowSurface(window);

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
        if (event.key.keysym.sym == SDLK_ESCAPE) { return -1; }
        if (event.key.keysym.sym == SDLK_TAB) { return 1; }
        break;

      case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_TAB) { return 2; }
        break;

        break;
      case SDL_QUIT:
        return -1;
    }
  }

#if 0
  const Uint8 *keystate = SDL_GetKeyboardState(NULL);

  //if (keystate[SDLK_ESCAPE])
  if (keystate[SDL_SCANCODE_ESCAPE])
  {
    return -1;
  }
#endif

  return 0;
}

