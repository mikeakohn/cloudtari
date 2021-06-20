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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ROM.h"

ROM::ROM() : size(0)
{
  memset(memory, 0, sizeof(memory));
}

ROM::~ROM()
{
}

int ROM::load(const char *filename)
{
  struct stat statbuf;
  memset(&statbuf, 0, sizeof(statbuf));
  stat(filename, &statbuf);

  if (statbuf.st_size != 2048 &&
      statbuf.st_size != 4096 &&
      statbuf.st_size != 8192)
  {
    printf("Error: ROM size is not 2048, 4096, 8192 (%ld)\n", statbuf.st_size);
    return -1;
  }

  FILE *in = fopen(filename, "rb");

  if (in == NULL)
  {
    printf("Error: Couldn't open file %s\n", filename);
    return -1;
  }

  size = fread(full, 1, statbuf.st_size, in);
  fclose(in);

  if (size != statbuf.st_size)
  {
    printf("Error: Could not load ROM %d/%ld\n", size, statbuf.st_size);
    return -2;
  }

  if (size == 2048)
  {
    memcpy(memory + 2048, full, 2048);
  }
    else
  {
    memcpy(memory, full, 4096);
  }

  return 0;
}

bool ROM::set_bank(int value)
{
  if (size < 8192) { return false; }

  memcpy(memory, full + (value * 4096), 4096);

  return true;
}

