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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "ColorTable.h"
#include "TelevisionVNC.h"

TelevisionVNC::TelevisionVNC() :
  socket_id{-1},
  client{-1},
  port{5900},
  needs_color_table{true}
{
  image_packet_length = sizeof(ImagePacket) + (width * height * 4);
  image_packet = (ImagePacket *)malloc(image_packet_length);

  memset(image_packet, 0, image_packet_length);

  image_packet->number_of_rectangles = htons(1);
  image_packet->width = htons(width);
  image_packet->height = htons(height);
  image_packet->encoding_type = htonl(ENCODING_RAW);

  memset(&refresh_time, 0, sizeof(refresh_time));
}

TelevisionVNC::~TelevisionVNC()
{
  close_connection();
  free(image_packet);
}

int TelevisionVNC::init()
{
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;

  socket_id = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_id < 0)
  {
    printf("Can't open socket.\n");
    return -1;
  }

  memset((char*)&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  if (bind(socket_id, (const sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("Server can't bind.\n");
    return -1;
  }

  if (listen(socket_id, 1) != 0)
  {
    printf("Listen failed.\n");
    return -1;
  }

  socklen_t n = sizeof(client_addr);

  client = accept(socket_id, (struct sockaddr *)&client_addr, &n);

  if (client == -1) { return -1; }

  fcntl(client, F_SETFL, O_NONBLOCK);

  if (send_protocol_version() != 0) { return -1; }
  if (get_client_protocol_version() != 0) { return -1; }
  if (send_security() != 0) { return -1; }
  if (get_client_init() != 0) { return -1; }
  if (send_server_init() != 0) { return -1; }
  //if (send_color_table() != 0) { return -1; }

  return 0;
}

void TelevisionVNC::clear_display()
{
  const int length = width * height * 4;

  memset(image_packet->data, 0, length);
}

void TelevisionVNC::draw_pixel(int x, int y, uint32_t color)
{
  int pixel;

  x = x * 3;
  y = y * 2;

  if (x < 0 || y < 0) { return; }

  pixel = (y * width) + x;

  if (pixel < width * height + width + 1)
  { 
    image_packet->data[pixel + 0] = color;
    image_packet->data[pixel + 1] = color;
    image_packet->data[pixel + 2] = color;
    image_packet->data[pixel + width + 0] = color;
    image_packet->data[pixel + width + 1] = color;
    image_packet->data[pixel + width + 2] = color;
  }
}

bool TelevisionVNC::refresh()
{
  struct timeval now;

  gettimeofday(&now, NULL);
  long time_diff = now.tv_usec - refresh_time.tv_usec;
  while(time_diff < 0) { now.tv_sec--; time_diff += 1000000; }
  time_diff += (now.tv_sec - refresh_time.tv_sec) * 1000000;

  if (time_diff < 16000)
  {
    usleep(16000 - time_diff);
  }

  refresh_time = now;

  send_image_full();

  return true;
}

int TelevisionVNC::handle_events()
{
  uint8_t buffer[128];
  uint32_t key;
  int n, count;

  if (vnc_has_data())
  {
    uint8_t message_type = 0xff;

    vnc_recv(&message_type, 1);

    switch (message_type)
    {
      case 0:
        printf("From Client: SetPixelFormat\n");
        vnc_recv(buffer + 1, 19);
        print_pixel_format(buffer);
        break;
      case 2:
        printf("From Client: SetEncodings\n");
        vnc_recv(buffer, 3);
        count = (buffer[1] << 8) | buffer[2];

        for (n = 0; n < count; n++) { vnc_recv(buffer, 4); }
        break;
      case 3:
        printf("From Client: FramebufferUpdateRequest\n");
        vnc_recv(buffer + 1, 9);
        send_image_update(
          (buffer[2] << 8) | buffer[3],
          (buffer[4] << 8) | buffer[5],
          (buffer[6] << 8) | buffer[7],
          (buffer[8] << 8) | buffer[9]);
        break;
      case 4:
        printf("From Client: KeyEvent\n");
        vnc_recv(buffer + 1, 7);
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
        printf("From Client: PointerEvent\n");
        vnc_recv(buffer + 1, 5);
        break;
      case 6:
        printf("From Client: ClientCutText\n");
        vnc_recv(buffer + 1, 7);
        count =
          (buffer[4] << 24) |
          (buffer[5] << 16) |
          (buffer[6] << 8) |
           buffer[7];
        // FIXME: This is not efficient, but don't really expect this one.
        for (n = 0; n < count; n++) { vnc_recv(buffer, 1); }
        break;
    }
  }

  return 0;
}

int TelevisionVNC::vnc_send(const uint8_t *buffer, int length)
{
  struct timeval tv;
  fd_set writeset;

  int bytes_sent = 0;

  while (bytes_sent < length)
  {
    FD_ZERO(&writeset);
    FD_SET(client, &writeset);

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    int n = select(client + 1, NULL, &writeset, NULL, &tv);

    if (n == -1)
    {
      if (errno == EINTR) { continue; }

      perror("Problem with select in vnc_send");
      return -1;
    }

    if (n == 0) { return -3; }

    n = send(client, buffer + bytes_sent, length - bytes_sent, 0);
    if (n < 0) { return -4; }

    bytes_sent += n;
  }

  return bytes_sent;
}

int TelevisionVNC::vnc_recv(uint8_t *buffer, int length)
{
  struct timeval tv;
  fd_set readset;

  int bytes_received = 0;

  while (bytes_received < length)
  {
    FD_ZERO(&readset);
    FD_SET(client, &readset);

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    int n = select(client + 1, &readset, NULL, NULL, &tv);

    if (n == -1)
    {
      if (errno == EINTR) { continue; }

      perror("Problem with select in vnc_recv");
      return -1;
    }

    if (n == 0) { return -3; }

    n = recv(client, buffer + bytes_received, length - bytes_received, 0);
    if (n < 0) { return -4; }

    bytes_received += n;
  }

  return bytes_received;
}

bool TelevisionVNC::vnc_has_data()
{
  struct timeval tv;
  fd_set readset;

  FD_ZERO(&readset);
  FD_SET(client, &readset);

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  int n = select(client + 1, &readset, NULL, NULL, &tv);

  if (n == -1)
  {
    if (errno == EINTR) { return false; }

    perror("Problem with select in vnc_recv");
    close_connection();
    return false;
  }

  return FD_ISSET(client, &readset);
}

void TelevisionVNC::close_connection()
{
  if (socket_id == -1) { return; }
  close(socket_id);
  socket_id = -1;
}

int TelevisionVNC::send_protocol_version()
{
  const char *version = "RFB 003.003\n";

  if (vnc_send((const uint8_t *)version, strlen(version)) != 12)
  {
    printf("Couldn't send 12 bytes\n");
    return -1;
  }

  return 0;
}

int TelevisionVNC::get_client_protocol_version()
{
  uint8_t version[12];

  if (vnc_recv(version, sizeof(version)) != 12)
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

  vnc_send(security, sizeof(security));
  //vnc_send(success, sizeof(security));

  return 0;
}

int TelevisionVNC::get_client_init()
{
  uint8_t init[1] = { 0 };

  if (vnc_recv(init, sizeof(init)) != 1)
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

  if (vnc_send((const uint8_t *)&packet, sizeof(packet)) != 32)
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

  if (vnc_send((const uint8_t *)&packet, sizeof(packet)) != sizeof(packet))
  {
    printf("Error: Send packet %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  return 0;
}

int TelevisionVNC::send_image_full()
{
  if (needs_color_table) { return 0; }

  if (vnc_send((const uint8_t *)image_packet, image_packet_length) != image_packet_length)
  {
    printf("Error: Send packet %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  return 0;
}

int TelevisionVNC::send_image_update(int x, int y, int width, int height)
{
  if (needs_color_table)
  {
    //if (send_color_table() != 0) { return -1; }
    needs_color_table = false;
  }

  //printf("send_image_update(%d, %d, %d, %d)\n", x, y, width, height);

  if (x == 0 && y == 0 && width == this->width && height == this->height)
  {
    send_image_full();
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

