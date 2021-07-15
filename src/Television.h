/**
 *  Cloudtari
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2021 by Michael Kohn
 *
 * Television is an abstract (pure virtual) class that needs to be
 * extended by the different ways there are to display the game,
 * in this case being: Null (nothing), SDL (on screen), VNC (remote
 * desktop), or Http (webbrowser / GIFs).
 *
 */

#ifndef TELEVISION_H
#define TELEVISION_H

#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

class Television
{
public:
  Television();
  virtual ~Television();

  virtual int init() = 0;
  //virtual void clear_display() = 0;
  //virtual void draw_pixel(int x, int y, uint32_t color) = 0;
  //virtual void draw_pixel(int x, int y, uint8_t color) = 0;
  virtual bool refresh() = 0;
  virtual int handle_events() = 0;
  virtual void *get_image() = 0;
  virtual int get_bitsize() = 0;
  virtual void set_port(int value) { }
  int get_width() { return width; }
  int get_height() { return height; }

  void pause()
  {
    struct timeval now;

    gettimeofday(&now, NULL);
    long time_diff = now.tv_usec - refresh_time.tv_usec;
    while (time_diff < 0) { now.tv_sec--; time_diff += 1000000; }
    time_diff += (now.tv_sec - refresh_time.tv_sec) * 1000000;

#if 0
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long time_diff = now.tv_nsec - refresh_time.tv_nsec;
    while (time_diff < 0) { now.tv_sec--; time_diff += 1000000000; }
    time_diff += (now.tv_sec - refresh_time.tv_sec) * 1000000000;
    time_diff = time_diff / 1000;
#endif

    // 1,000,000 us / 60 fps = 16667
    // 1,000,000 us / 30 fps = 33333
    // FIXME: The TV should be 60fps, but for some reason 33333
    // is coming out at 60fps in the TIA.
    if (time_diff < 33333)
    {
      usleep(33333 - time_diff);
    }

    refresh_time = now;
  }

  enum
  {
    KEY_QUIT = 1,
    KEY_SELECT_DOWN,
    KEY_SELECT_UP,
    KEY_RESET_DOWN,
    KEY_RESET_UP,
    KEY_LEFT_DOWN,
    KEY_LEFT_UP,
    KEY_RIGHT_DOWN,
    KEY_RIGHT_UP,
    KEY_UP_DOWN,
    KEY_UP_UP,
    KEY_DOWN_DOWN,
    KEY_DOWN_UP,
    KEY_FIRE_DOWN,
    KEY_FIRE_UP,
  };

protected:
  int width, height;
  struct timeval refresh_time;

private:

};

#endif

