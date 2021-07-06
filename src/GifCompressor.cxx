/**
 *  Cloudtari
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2021 by Michael Kohn
 *
 * This is some old C code from 2005 or so translated to C++.
 * https://www.mikekohn.net/file_formats/libkohn_gif.php
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GifCompressor.h"

GifCompressor::GifCompressor() : gif_length{0}
{
  data_length = 65536;
  data = (uint8_t *)malloc(data_length);

  memset(data, 0, data_length);

  memset(&gif_header, 0, sizeof(gif_header));
  memcpy(gif_header.version, "GIF87a", 6);
  memcpy(data, "GIF87a", 6);
}

GifCompressor::~GifCompressor()
{
}

int GifCompressor::compress(uint8_t *image, uint32_t *color_table)
{
  int ptr = 6, i;

  // Remap the colors from the 128 color palette to a smaller palette
  // so the GIF compresses more.
  uint8_t color_map[256];
  uint8_t color_used[256];
  uint32_t gif_palette[256];

  memset(color_map, 0, sizeof(color_map));
  memset(color_used, 0, sizeof(color_used));

  int length = gif_header.width * gif_header.height;
  int max_colors = 0;

  // Remap colors to GIF palette.
  for (i = 0; i < length; i++)
  {
    int color = image[i];

    if (color_used[color]) { continue; }

    color_map[color] = max_colors;
    color_used[color] = 1;
    gif_palette[max_colors] = color_table[color];

    max_colors++;
  }

  // FIXME: Why is this happening?
  if (max_colors < 2) { max_colors = 4; }

  // Compute bits per pixel and max colors for that.
  int bits_per_pixel = compute_bits_per_pixel(max_colors);
  const int color_resolution = bits_per_pixel;
  max_colors = 1 << bits_per_pixel;
//printf("max_colors=%d bits_per_pixel=%d\n", max_colors, bits_per_pixel);

  // Copy GifHeader (Logical Screen Descriptor) to memory.
  set_uint16(data, ptr + 0, gif_header.width);
  set_uint16(data, ptr + 2, gif_header.height);
  data[ptr + 4] = 0x80 | ((color_resolution - 1) << 4) | (bits_per_pixel - 1);
  data[ptr + 5] = 0;
  data[ptr + 6] = 0;
  ptr += 7;

  // Global Color Map (palettes).
  for (i = 0; i < max_colors; i++)
  {
    data[ptr++] = (gif_palette[i] >> 16) & 0xff;
    data[ptr++] = (gif_palette[i] >> 8) & 0xff;
    data[ptr++] =  gif_palette[i] & 0xff;
  }

  // Image Descriptor Block.
  data[ptr + 0] = ',';
  set_uint16(data, ptr + 1, 0);
  set_uint16(data, ptr + 3, 0);
  set_uint16(data, ptr + 5, gif_header.width);
  set_uint16(data, ptr + 7, gif_header.height);
  data[ptr + 9] = bits_per_pixel - 1;
  ptr += 10;

  int code_size = bits_per_pixel;
  int clear_code = max_colors;
  int eof_code = max_colors + 1;

  data[ptr++] = code_size;

  // Setup LZW tree.
  CompressNode node[4097];

  for (i = 0; i < max_colors; i++)
  {
    node[i].color = i;
    node[i].right = i + 1;
    node[i].down = -1;
  }

  // Compressed data blocks follow.
  int table_start_size = max_colors + 2;
  int next_code = table_start_size;
  int curr_code_size = code_size + 1;
  int curr_code = -1;
  int data_size = 0;

  // LZW Compression.
  BitStream bit_stream;

  bit_stream.append(clear_code, curr_code_size);

  int image_ptr = 0;
  uint8_t color = color_map[image[image_ptr++]];
  int block_ptr;

  block_ptr = ptr;
  data[ptr++] = 0;

  curr_code = color;
  int last_code = curr_code;

  while (image_ptr < length)
  {
    color = color_map[image[image_ptr++]];

    if (node[curr_code].down == -1)
    {
      last_code = curr_code;
      node[curr_code].down = next_code;
    }
      else
    {
      last_code = curr_code;
      curr_code = node[curr_code].down;

      while (node[curr_code].color != color && node[curr_code].right != -1)
      {
        curr_code=node[curr_code].right;
      }

      if (node[curr_code].color != color && node[curr_code].right == -1)
      {
        node[curr_code].right=next_code;
      }
        else
      {
        continue;
      }
    }

    bit_stream.append(last_code, curr_code_size);

    node[next_code].right = -1;
    node[next_code].down = -1;
    node[next_code].color = color;
    curr_code = color;

    if ((next_code >> curr_code_size) != 0)
    {
      if (curr_code_size >= 12)
      {
        bit_stream.append(clear_code, curr_code_size);

        for (i = 0; i < max_colors; i++)
        {
          node[i].down = -1;
        }

        next_code = table_start_size - 1;
        curr_code_size = code_size;
      }

      curr_code_size++;
    }

    next_code++;

    while (bit_stream.size() >= 8)
    {
      data[ptr++] = bit_stream.get_byte();
      data_size++;

      if (data_size >= 255)
      {
        data[block_ptr] = data_size;
        data_size = 0;
        block_ptr = ptr;
        data[ptr++] = 0;
      }
    }
  }

  bit_stream.append(curr_code, curr_code_size);
  bit_stream.append(eof_code, curr_code_size);

  while (bit_stream.size() > 0)
  {
    data[ptr++] = bit_stream.get_byte();
    data_size++;

    if (data_size >= 255)
    {
      data[block_ptr] = data_size;
      data_size = 0;
      block_ptr = ptr;
      data[ptr++] = 0;
    }
  }

  if (data_size > 0)
  {
    data[block_ptr] = data_size;
  }

  // End Marker.
  data[ptr++] = 0;
  data[ptr++] = ';';

  gif_length = ptr;

  return 0;
}

int GifCompressor::compute_bits_per_pixel(int max_colors)
{
  uint8_t counts[16] =
  {
    0, 1, 2, 2, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
  };

  max_colors -= 1;

  int upper = counts[max_colors >> 4];
  int lower = counts[max_colors & 0xf];

  if (upper != 0) { return upper + 4; }
  if (lower == 0) { lower = 1; }

  return lower;
}

