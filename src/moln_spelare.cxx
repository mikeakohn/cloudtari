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

int main(int argc, char *argv[])
{
  FILE *in;

  if (argc != 2)
  {
    printf("Usage: %s <gamefile.bin>\n", argv[0]);
    exit(0);
  }

  in = fopen(argv[1], "rb");

  if (in == NULL)
  {
    printf("Couldn't open file %s\n", argv[1]);
    exit(-1);
  }

  fclose(in);

  return 0;
}

