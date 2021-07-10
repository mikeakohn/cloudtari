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

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <time.h>

class Timer
{
public:
  Timer() { }
  ~Timer() { }

  inline void cpu_start()
  {
#if defined(__x86_64__)
    asm __volatile__
    (
      "rdtsc" : "=a" (cycle_start.split.lo), "=d" (cycle_start.split.hi)
    );
#else
    memset(&cycle_start, 0, sizeof(cycle_start));
#endif
  }

  inline void cpu_stop()
  {
#if defined(__x86_64__)
    asm __volatile__
    (
      "rdtsc" : "=a" (cycle_stop.split.lo), "=d" (cycle_stop.split.hi)
    );
#else
    memset(&cycle_stop, 0, sizeof(cycle_stop));
#endif

    printf("cpu=%ld\n", cycle_stop.count - cycle_start.count);
    fflush(stdout);
  }

  inline void time_start()
  {
    clock_gettime(CLOCK_MONOTONIC, &tp_start);
  }

  inline void time_stop()
  {
    clock_gettime(CLOCK_MONOTONIC, &tp_stop);

    long nsec = tp_stop.tv_nsec - tp_start.tv_nsec;
    long sec = tp_stop.tv_sec - tp_start.tv_sec;

    if (nsec < 0) { sec--; nsec += 1000000000; }

    double t = (sec * 1000) + ((double)nsec / 1000000);

    printf("time=%f ms\n", t);
    fflush(stdout);
  }

private:
  union
  {
    struct _split
    {
      uint32_t lo;
      uint32_t hi;
    } split;
    uint64_t count;
  } cycle_start, cycle_stop;

  struct timespec tp_start;
  struct timespec tp_stop;
};

#endif

