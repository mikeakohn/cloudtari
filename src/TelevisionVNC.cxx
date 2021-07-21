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
//#include <unistd.h>
#include <arpa/inet.h>

#include "ColorTable.h"
#include "TelevisionVNC.h"

TelevisionVNC::TelevisionVNC() :
  needs_full_image{true},
  needs_color_table{true},
  image_page{0}
{
  image_packet_length = sizeof(ImagePacket) + (width * height * 4);
  image_packet[0] = (ImagePacket *)malloc(image_packet_length);
  image_packet[1] = (ImagePacket *)malloc(image_packet_length);

  memset(image_packet[0], 0, image_packet_length);
  memset(image_packet[1], 0, image_packet_length);

  image_packet[0]->number_of_rectangles = htons(1);
  image_packet[0]->width = htons(width);
  image_packet[0]->height = htons(height);
  image_packet[0]->encoding_type = htonl(ENCODING_RAW);

  image_packet[1]->number_of_rectangles = htons(1);
  image_packet[1]->width = htons(width);
  image_packet[1]->height = htons(height);
  image_packet[1]->encoding_type = htonl(ENCODING_RAW);

  diff_buffer_length = width * height * 4;
  diff_buffer = (uint8_t *)malloc(diff_buffer_length);
}

TelevisionVNC::~TelevisionVNC()
{
  net_close();
  free(image_packet[0]);
  free(image_packet[1]);
}

int TelevisionVNC::init()
{
  if (net_open(port) != 0) { return -1; }

  if (send_protocol_version() != 0) { return -1; }
  if (get_client_protocol_version() != 0) { return -1; }
  if (send_security() != 0) { return -1; }
  if (get_client_init() != 0) { return -1; }
  if (send_server_init() != 0) { return -1; }
  //if (send_color_table() != 0) { return -1; }

  return 0;
}

#if 0
void TelevisionVNC::clear_display()
{
  const int length = width * height * 4;

  memset(image_packet[image_page]->data, 0, length);
}

void TelevisionVNC::draw_pixel(int x, int y, uint32_t color)
{
  set_pixel(x, y, color);
}

void TelevisionVNC::draw_pixel(int x, int y, uint8_t color)
{
  set_pixel(x, y, ColorTable::get_color(color));
}
#endif

bool TelevisionVNC::refresh()
{
  pause();

  send_image_diff();

  image_page ^= 1;

  return true;
}

int TelevisionVNC::handle_events()
{
  uint8_t buffer[128];
  uint32_t key;
  int n, count;

  if (net_has_data())
  {
    uint8_t message_type = 0xff;

    net_recv(&message_type, 1);

    switch (message_type)
    {
      case 0:
        printf("From Client: SetPixelFormat\n");
        net_recv(buffer + 1, 19);
        print_pixel_format(buffer);
        break;
      case 2:
        printf("From Client: SetEncodings\n");
        net_recv(buffer, 3);
        count = (buffer[1] << 8) | buffer[2];

        for (n = 0; n < count; n++)
        {
          net_recv(buffer, 4);
          print_encoding(buffer);
        }
        break;
      case 3:
        //printf("From Client: FramebufferUpdateRequest\n");
        net_recv(buffer + 1, 9);
        send_image_update(
          (buffer[2] << 8) | buffer[3],
          (buffer[4] << 8) | buffer[5],
          (buffer[6] << 8) | buffer[7],
          (buffer[8] << 8) | buffer[9],
          buffer[1]);
        break;
      case 4:
        //printf("From Client: KeyEvent\n");
        net_recv(buffer + 1, 7);
        key =
          (buffer[4] << 24) |
          (buffer[5] << 16) |
          (buffer[6] << 8) |
           buffer[7];

        // If buffer[1] is not 0 then it's a keydown.
        if (buffer[1] != 0)
        {
          // Escape key quits the game.
          if (key == 0xff1b) { return KEY_QUIT; }

          // Tab key was pressed.
          if (key == 0xff09) { return KEY_SELECT_DOWN; }
          if (key == 0xff0d) { return KEY_RESET_DOWN; }
          if (key == 0xff51) { return KEY_LEFT_DOWN; }
          if (key == 0xff54) { return KEY_UP_DOWN; }
          if (key == 0xff53) { return KEY_RIGHT_DOWN; }
          if (key == 0xff54) { return KEY_DOWN_DOWN; }
          if (key == ' ') { return KEY_FIRE_DOWN; }
        }
          else
        {
          // Tab key was released.
          if (key == 0xff09) { return KEY_SELECT_UP; }
          if (key == 0xff0d) { return KEY_RESET_UP; }
          if (key == 0xff51) { return KEY_LEFT_UP; }
          if (key == 0xff54) { return KEY_UP_UP; }
          if (key == 0xff53) { return KEY_RIGHT_UP; }
          if (key == 0xff54) { return KEY_DOWN_UP; }
          if (key == ' ') { return KEY_FIRE_UP; }
        }

        break;
      case 5:
        //printf("From Client: PointerEvent\n");
        net_recv(buffer + 1, 5);
        break;
      case 6:
        printf("From Client: ClientCutText\n");
        net_recv(buffer + 1, 7);
        count =
          (buffer[4] << 24) |
          (buffer[5] << 16) |
          (buffer[6] << 8) |
           buffer[7];
        // FIXME: This is not efficient, but don't really expect this one.
        for (n = 0; n < count; n++) { net_recv(buffer, 1); }
        break;
    }
  }

  return 0;
}

void TelevisionVNC::set_pixel(int x, int y, uint32_t color)
{
  int pixel;

  x = x * 3;
  y = y * 2;

  if (x < 0 || y < 0) { return; }

  pixel = (y * width) + x;

  if (pixel < width * height + width + 1)
  {
    image_packet[image_page]->data[pixel + 0] = color;
    image_packet[image_page]->data[pixel + 1] = color;
    image_packet[image_page]->data[pixel + 2] = color;
    image_packet[image_page]->data[pixel + width + 0] = color;
    image_packet[image_page]->data[pixel + width + 1] = color;
    image_packet[image_page]->data[pixel + width + 2] = color;
  }
}

int TelevisionVNC::send_protocol_version()
{
  const char *version = "RFB 003.003\n";

  if (net_send((const uint8_t *)version, strlen(version)) != 12)
  {
    printf("Couldn't send 12 bytes\n");
    return -1;
  }

  return 0;
}

int TelevisionVNC::get_client_protocol_version()
{
  uint8_t version[12];

  if (net_recv(version, sizeof(version)) != 12)
  {
    printf("Error: Client protocol version.\n");
    return -1;
  }

  printf("%s\n", version);

  return 0;
}

int TelevisionVNC::send_security()
{
  // No password.
  const uint8_t security[] = { 0, 0, 0, 1 };
  //const uint8_t success[] = { 0, 0, 0, 0 };

  net_send(security, sizeof(security));
  //net_send(success, sizeof(security));

  return 0;
}

int TelevisionVNC::get_client_init()
{
  uint8_t init[1] = { 0 };

  if (net_recv(init, sizeof(init)) != 1)
  {
    printf("Error: Client init improper response.\n");
    return -1;
  }

  printf("Client init response %d\n", init[0]);

  return 0;
}

int TelevisionVNC::send_server_init()
{
  struct
  {
    uint16_t width;
    uint16_t height;
    uint8_t bits_per_pixel;
    uint8_t depth;
    uint8_t big_endian_flag;
    uint8_t true_colour_flag;
    uint16_t red_max;
    uint16_t green_max;
    uint16_t blue_max;
    uint8_t red_shift;
    uint8_t green_shift;
    uint8_t blue_shift;
    uint8_t padding[3];
    uint32_t name_length;
    char name[8];
  } packet;

  memset(&packet, 0, sizeof(packet));

  packet.width = htons(width);
  packet.height = htons(height);
  packet.bits_per_pixel = 32;
  packet.depth = 32;
  //packet.bits_per_pixel = 8;
  //packet.depth = 8;
  //packet.big_endian_flag = 1;
  packet.true_colour_flag = 1;
  packet.red_max = htons(255);
  packet.green_max = htons(255);
  packet.blue_max = htons(255);
  packet.red_shift = 16;
  packet.green_shift = 8;
  packet.blue_shift = 0;
  packet.name_length = htonl(8);
  memcpy(packet.name, "ATARI---", 8);

  if (sizeof(packet) != 32)
  {
    printf("Error: Size of packet %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  if (net_send((const uint8_t *)&packet, sizeof(packet)) != 32)
  {
    printf("Error: Send packet %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  return 0;
}

int TelevisionVNC::send_color_table()
{
  struct
  {
    uint8_t message_type;
    uint8_t padding;
    uint16_t first_colour;
    uint16_t number_of_colours;
    uint16_t table[128 * 3];
  } packet;

  memset(&packet, 0, sizeof(packet));

  if (sizeof(packet) != 768 + 6)
  {
    printf("Error: sizeof() error  %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  packet.message_type = 1;
  packet.number_of_colours = htons(128);

  for (int n = 0; n < 128; n++)
  {
    uint32_t color = ColorTable::get_color(n);
    uint16_t red = color >> 16;
    uint16_t green = (color >> 8) & 0xff;
    uint16_t blue = color & 0xff;

    int index = n * 3;
    packet.table[index + 0] = htons(red);
    packet.table[index + 1] = htons(green);
    packet.table[index + 2] = htons(blue);
  }

  if (net_send((const uint8_t *)&packet, sizeof(packet)) != sizeof(packet))
  {
    printf("Error: Send packet %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  return 0;
}

int TelevisionVNC::send_image_full()
{
  //if (needs_color_table) { return 0; }

  if (net_send((const uint8_t *)image_packet[image_page], image_packet_length) != image_packet_length)
  {
    printf("Error: Send packet %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  needs_full_image = false;

  return 0;
}

int TelevisionVNC::send_image_diff()
{
  int line_mismatch_count = 0;
  int old_page = image_page ^ 1;
  int mismatch_start = 0;
  bool in_mismatch = false;
  int diff_ptr = sizeof(FramebufferUpdate);
  FramebufferUpdate *frame_buffer_update = (FramebufferUpdate *)diff_buffer;

  memset(frame_buffer_update, 0, sizeof(FramebufferUpdate));

  for (int y = 0; y < height; y += 2)
  {
    int line = y * width;
    bool is_mismatch = false;

    for (int x = 0; x < width; x += 3)
    {
      is_mismatch =
        image_packet[image_page]->data[line + x] !=
        image_packet[old_page]->data[line + x];

      if (is_mismatch)
      {
        line_mismatch_count++;

        if (line_mismatch_count > height / 6) { return send_image_full(); }
        if (!in_mismatch) { mismatch_start = y; in_mismatch = true; }

        break;
      }
    }

    // If mismatch flag is set and this is the last line in the image or
    // no longer in a mismatch.
    if (in_mismatch)
    {
      if (y == height - 2 || is_mismatch == false)
      {
        frame_buffer_update->number_of_rectangles++;

        UpdateRectangle *update_rectangle =
          (UpdateRectangle *)(diff_buffer + diff_ptr);

        int copy_height = y - mismatch_start;

        if (y == height - 2)
        {
          copy_height = height - mismatch_start;
        }

        update_rectangle->x = htons(0);
        update_rectangle->y = htons(mismatch_start);
        update_rectangle->width = htons(width);
        update_rectangle->height = htons(copy_height);
        update_rectangle->encoding_type = htons(0);

        diff_ptr += sizeof(UpdateRectangle);

        int copy_size = width * copy_height * 4;

        if (diff_ptr + copy_size > diff_buffer_length)
        {
          return send_image_full();
        }

        memcpy(
          diff_buffer + diff_ptr,
          &image_packet[image_page]->data[mismatch_start * width],
          copy_size);

        diff_ptr += copy_size;

        in_mismatch = false;
      }
    }
  }

  if (line_mismatch_count == 0) { return 0; }

  frame_buffer_update->number_of_rectangles =
    htons(frame_buffer_update->number_of_rectangles);

  if (net_send(diff_buffer, diff_ptr) != diff_ptr)
  {
    printf("Error: Send packet %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  return 0;
}

int TelevisionVNC::send_image_update(
  int x,
  int y,
  int width,
  int height,
  bool incremental)
{
  if (needs_color_table)
  {
    needs_color_table = false;
  }

  //printf("send_image_update(%d, %d, %d, %d)\n", x, y, width, height);

  if (x == 0 && y == 0 && width == this->width && height == this->height)
  {
    if (incremental) { return 0; }
    needs_full_image = true;
  }

  return 0;
}

void TelevisionVNC::print_pixel_format(uint8_t *buffer)
{
  buffer += 4;

  printf("  bits_per_pixel: %d\n", buffer[0]);
  printf("           depth: %d\n", buffer[1]);
  printf("      big_endian: %d\n", buffer[2]);
  printf("     true_colour: %d\n", buffer[3]);
  printf("         red_max: %d\n", (buffer[4] << 8) | buffer[5]);
  printf("       green_max: %d\n", (buffer[6] << 8) | buffer[7]);
  printf("        blue_max: %d\n", (buffer[8] << 8) | buffer[9]);
  printf("       red_shift: %d\n", buffer[10]);
  printf("     green_shift: %d\n", buffer[11]);
  printf("      blue_shift: %d\n", buffer[12]);
}

void TelevisionVNC::print_encoding(uint8_t *buffer)
{
  int *i = (int *)buffer;
  int encoding = htonl(*i);

  switch (encoding)
  {
    case 0: printf("  %d Raw\n", encoding); break;
    case 1: printf("  %d CopyRect\n", encoding); break;
    case 2: printf("  %d RRE\n", encoding); break;
    case 4: printf("  %d CoRRE\n", encoding); break;
    case 5: printf("  %d Hextile\n", encoding); break;
    case 7: printf("  %d tight\n", encoding); break;
    case 8: printf("  %d zlibhex\n", encoding); break;
    case 15: printf("  %d TRLE\n", encoding); break;
    case 16: printf("  %d ZRLE\n", encoding); break;
    case 17: printf("  %d Hitachi ZYWRLE\n", encoding); break;
    case -239: printf("  %d Cursor\n", encoding); break;
    case -223: printf("  %d DesktopSize\n", encoding); break;
    default: printf("  %d Unknown\n", encoding); break;
  }
}

