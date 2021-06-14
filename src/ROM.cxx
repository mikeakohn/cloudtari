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

ROM::ROM()
{
  memset(memory, 0, sizeof(memory));
}

ROM::~ROM()
{
}

int ROM::load(const char *filename)
{
  FILE *in = fopen(filename, "rb");
  struct stat statbuf;
  int len = -1;

  if (in == NULL)
  {
    printf("Error: Couldn't open file %s\n", filename);
    return -1;
  }

  stat(filename, &statbuf);

  if (statbuf.st_size == 4096)
  {
    len = fread(memory, 1, statbuf.st_size, in);
  }
    else
  if (statbuf.st_size == 2048)
  {
    len = fread(memory + 2048, 1, statbuf.st_size, in);
  }

  fclose(in);

  if (len != statbuf.st_size)
  {
    printf("Error: ROM size is not 2048 or 4096\n");
    fclose(in);
    return -2;
  }

  return 0;
}

