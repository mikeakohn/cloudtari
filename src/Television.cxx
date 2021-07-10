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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Television.h"

Television::Television() : width{480}, height{384}
{
  memset(&refresh_time, 0, sizeof(refresh_time));
}

Television::~Television()
{
}

