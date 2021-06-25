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

#ifndef DEBUG_TIMER_H
#define DEBUG_TIMER_H

class DebugTimer
{
public:
  DebugTimer() :
    start_address{-1},
    end_address{-1},
    cycles{0},
    running{0}
  {
  }

  ~DebugTimer() { }

  void set(int start_address, int end_address)
  {
    this->start_address = start_address;
    this->end_address = end_address;
  }

  void compute(int address, int cycles)
  {
    if (!running)
    {
      if (address == start_address)
      {
        this->cycles += cycles;
        //printf("cycles %d / %d\n", this->cycles, cycles);
        running = true;
      }

      return;
    }

    this->cycles += cycles;
    //printf("cycles %d / %d\n", this->cycles, cycles);

    if (address == end_address)
    {
      running = false;

      printf("Timer 0x%04x-0x%04x, cycles=%d\n",
        start_address,
        end_address,
        this->cycles);

      start_address = -1;
      end_address = -1;
      cycles = 0;
    }
  }

private:
  int start_address;
  int end_address;
  int cycles;
  bool running;
};

#endif

