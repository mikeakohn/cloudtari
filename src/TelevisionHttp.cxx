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
#include <unistd.h>
#include <sys/time.h>
#include <string>

#include "ColorTable.h"
#include "TelevisionHttp.h"

TelevisionHttp::TelevisionHttp() : gif{nullptr}, gif_length{0}
{
  gif_compressor = new GifCompressor();
  gif_compressor->set_width(width);
  gif_compressor->set_height(height);

  image = (uint8_t *)malloc(width * height);

  memset(&refresh_time, 0, sizeof(refresh_time));
  memset(filename, 0, sizeof(filename));
  memset(query_string, 0, sizeof(query_string));
}

TelevisionHttp::~TelevisionHttp()
{
  net_close();
  free(image);

  delete gif_compressor;
}

int TelevisionHttp::init()
{
  if (net_open(port) != 0) { return -1; }
  if (read_http() != 0) { return -1; }
  if (send_index_html() != 0) { return -1; }

  return 0;
}

void TelevisionHttp::clear_display()
{
  memset(image, 0, width * height);
}

void TelevisionHttp::draw_pixel(int x, int y, uint32_t color)
{
  printf("Internal error: draw_pixel(int x, int y, uint32_t color) not implemented\n");

  exit(1);
}

void TelevisionHttp::draw_pixel(int x, int y, uint8_t color)
{
  int pixel;

  x = x * 3;
  y = y * 2;

  if (x < 0 || y < 0) { return; }

  pixel = (y * width) + x;

  color = color >> 1;

  if (pixel < width * height + width + 1)
  {
    image[pixel + 0] = color;
    image[pixel + 1] = color;
    image[pixel + 2] = color;
    image[pixel + width + 0] = color;
    image[pixel + width + 1] = color;
    image[pixel + width + 2] = color;
  }
}

bool TelevisionHttp::refresh()
{
  gif_compressor->compress(image, ColorTable::get_table());

  gif = gif_compressor->get_gif_data();
  gif_length = gif_compressor->get_gif_length();

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

  return true;
}

int TelevisionHttp::handle_events()
{
  if (net_has_data())
  {
    read_http();

    // The query string from the browser is ?keys#timestamp.
    // The timestamp is unfortunate since it seems the browser is ignoring
    // the cache expire headers.
    int n = 0;

    while (query_string[n] != 0)
    {
      if (query_string[n] == '#') { break; }
      key_queue.append(query_string[n]);
      n++;
    }

    if (strcmp(filename, "/") == 0)
    {
      send_index_html();
    }
      else
    if (strcmp(filename, "/image.gif") == 0)
    {
      send_gif();
    }
      else
    {
      send_404();
    }

    filename[0] = 0;
    query_string[0] = 0;
  }

  if (!key_queue.is_empty())
  {
    char c = key_queue.get_next();
    switch (c)
    {
      case 's': return KEY_DOWN_DOWN;
      case 'w': return KEY_UP_DOWN;
      case 'a': return KEY_LEFT_DOWN;
      case 'd': return KEY_RIGHT_DOWN;
      case 'f': return KEY_FIRE_DOWN;
      case 'e': return KEY_RESET_DOWN;
      case 'c': return KEY_SELECT_DOWN;
      case 'S': return KEY_DOWN_UP;
      case 'W': return KEY_UP_UP;
      case 'A': return KEY_LEFT_UP;
      case 'D': return KEY_RIGHT_UP;
      case 'F': return KEY_FIRE_UP;
      case 'E': return KEY_RESET_UP;
      case 'C': return KEY_SELECT_UP;
    }
  }

  return 0;
}

int TelevisionHttp::read_http()
{
  uint8_t buffer[8192];
  char line[1024];
  int ptr = 0, length;
  bool end_of_headers = false;

  while (!end_of_headers)
  {
    length = net_recv(buffer, sizeof(buffer), false);

    if (length <= 0 || length == 8192)
    {
      printf("Network receive error (return value %d).\n", length);
      return -1;
    }

    for (int n = 0; n < length; n++)
    {
      if (buffer[n] == '\r') { continue; }
      if (buffer[n] == '\n')
      {
        line[ptr] = 0;
        ptr = 0;

        if (line[0] == 0) { end_of_headers = true; }

        if (strncmp(line, "GET /", 5) == 0)
        {
//printf("line=%s\n", line);
          int ptr = 4;
          uint32_t n = 0;

          while (line[ptr] != 0)
          {
            if (line[ptr] == ' ') { break; }
            if (line[ptr] == '?') { break; }

            filename[n++] = line[ptr++];
            if (n >= sizeof(filename) - 1) { break; }
          }

          filename[n] = 0;
          n = 0;

          if (line[ptr++] == '?')
          {
            while (line[ptr] != 0)
            {
              if (line[ptr] == ' ') { break; }

              query_string[n++] = line[ptr++];
              if (n >= sizeof(query_string) - 1) { break; }
            }
          }

          query_string[n] = 0;
        }

        continue;
      }

      line[ptr++] = buffer[n];

      if (ptr == sizeof(line) - 1)
      {
        printf("Error: Http line too long (%d).\n", length);
        return -1;
      }
    }
  }

  return 0;
}

int TelevisionHttp::send_index_html()
{
  std::string header;
  const char *page =
    "<html>\n<body onload='init();' bgcolor='#000000'>\n"
    "<script type='text/javascript'>\n"
    "var queue = '';\n"
    "function init()\n"
    "{\n"
      "setTimeout(refresh_image, 2000);\n"
      "window.addEventListener('keydown', function(event)\n"
      "{\n"
        "if (event.defaultPrevented) { return; }\n"
        "switch(event.keyCode)\n"
        "{\n"
          "case 40: queue += 's'; break;\n"
          "case 38: queue += 'w'; break;\n"
          "case 37: queue += 'a'; break;\n"
          "case 39: queue += 'd'; break;\n"
          "case 32: queue += 'f'; break;\n"
          "case 13: queue += 'e'; break;\n"
          "case 67: queue += 'c'; break;\n"
          "default: break;\n"
        "}\n"
      "});\n"
      "window.addEventListener('keyup', function(event)\n"
      "{\n"
        "if (event.defaultPrevented) { return; }\n"
        "switch(event.keyCode)\n"
        "{\n"
          "case 40: queue += 'S'; break;\n"
          "case 38: queue += 'W'; break;\n"
          "case 37: queue += 'A'; break;\n"
          "case 39: queue += 'D'; break;\n"
          "case 32: queue += 'F'; break;\n"
          "case 13: queue += 'E'; break;\n"
          "case 67: queue += 'C'; break;\n"
          "default: break;\n"
        "}\n"
      "});\n"
    "}\n"
    "function refresh_image()\n"
    "{\n"
    //"console.log('refresh_image()');\n"
    "var atari = document.getElementById('atari');\n"
    "if (atari.complete)\n"
    "{\n"
    "atari.src = 'image.gif?' + queue + '#' + new Date().getTime();\n"
    "queue = '';\n"
    "}\n"
    "setTimeout(refresh_image, 33);\n"
    "}\n"
    "</script>\n"
    "<table width=100%% height=100%%>"
    "<tr><td width=100%% height=100%% align='center'>"
    "<img src='image.gif' id='atari'></td></tr>"
    "</body>\n</html>\n\n";

  header =
    "HTTP/1.1 200 OK\n"
    "Cache-Control: no-cache, must-revalidate\n"
    "Pragma: no-cache\n"
    "Content-Type: text/html\n"
    "Content-Length: " + std::to_string(strlen(page)) + "\n\n";

  net_send((uint8_t *)header.c_str(), header.size());
  net_send((uint8_t *)page, strlen(page));

  return 0;
}

int TelevisionHttp::send_gif()
{
  std::string header =
    "HTTP/1.1 200 OK\n"
    "Content-Type: image/gif\n"
    "Cache-Control: no-cache, must-revalidate\n"
    "Pragma: no-cache\n"
    "Content-Length: " + std::to_string(gif_length) + "\n\n";

  net_send((uint8_t *)header.c_str(), header.size());
  net_send(gif, gif_length);

  return 0;
}

int TelevisionHttp::send_404()
{
  const char *page = "<p>Not found</p>";

  std::string header =
    "HTTP/1.1 404 Not Found\n"
    "Content-Type: text/html\n"
    "Content-Length: " + std::to_string(strlen(page)) + "\n\n";

  net_send((uint8_t *)header.c_str(), header.size());
  net_send((uint8_t *)page, strlen(page));

  return 0;
}

