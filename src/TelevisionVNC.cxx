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

#include "TelevisionVNC.h"

TelevisionVNC::TelevisionVNC() :
  socket_id(-1),
  client(-1),
  port(5900)
{
  image_packet = (ImagePacket *)malloc(
    sizeof(ImagePacket) + width * height * sizeof(uint32_t));
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

  send_protocol_version();
  if (get_client_protocol_version() != 0) { return -1; }
  send_security();
  if (get_client_init() != 0) { return -1; }
  send_server_init();

  return 0;
}

void TelevisionVNC::clear_display()
{
  const int length = width * height * 3;

  memset(image_packet->data, 0, length);
}

void TelevisionVNC::draw_pixel(int x, int y, uint32_t color)
{
  int pixel;

  x = x * 2;
  y = y * 2;

  pixel = (y * width) + x;

  image_packet->data[pixel + 0] = color;
  image_packet->data[pixel + 1] = color;
  image_packet->data[pixel + width + 0] = color;
  image_packet->data[pixel + width + 1] = color;
}

bool TelevisionVNC::refresh()
{
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
printf("data is waiting %02x\n", message_type);

    switch (message_type)
    {
      case 0:
        printf("SetPixelFormat\n");
        vnc_recv(buffer, 19);
        break;
      case 2:
        printf("SetEncodings\n");
        vnc_recv(buffer, 3);
        count = (buffer[1] << 8) | buffer[2];

        for (n = 0; n < count; n++) { vnc_recv(buffer, 4); }
        break;
      case 3:
        printf("FramebufferUpdateRequest\n");
        vnc_recv(buffer + 1, 9);
        send_image_update(
          (buffer[2] << 8) | buffer[3],
          (buffer[4] << 8) | buffer[5],
          (buffer[6] << 8) | buffer[7],
          (buffer[8] << 8) | buffer[9]);
        break;
      case 4:
        printf("KeyEvent\n");
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
          if (key == 0xff1b) { return -1; }

          // Tab key was pressed.
          if (key == 0xff09) { return 1; }
        }
          else
        {
          // Tab key was released.
          if (key == 0xff09) { return 2; }
        }

        break;
      case 5:
        printf("PointerEvent\n");
        vnc_recv(buffer + 1, 5);
        break;
      case 6:
        printf("ClientCutText\n");
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

  tv.tv_sec = 10;
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
  packet.depth = 24;
  packet.big_endian_flag = 1;
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

int TelevisionVNC::send_image_full()
{
  return 0;
}

int TelevisionVNC::send_image_update(int x, int y, int width, int height)
{
  printf("send_image_update(%d, %d, %d, %d)\n", x, y, width, height);

  return 0;
}

