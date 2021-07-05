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

  return true;
}

int TelevisionHttp::handle_events()
{
  if (net_has_data())
  {
static int blah = 0;
printf("handle_event() %d\n", blah++);
    read_http();

printf("filename=%s\n", filename);

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
printf("send 404\n");
      send_404();
    }

    filename[0] = 0;
    query_string[0] = 0;
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
printf("line=%s\n", line);
        ptr = 0;

        if (line[0] == 0) { end_of_headers = true; }

        if (strncmp(line, "GET /", 5) == 0)
        {
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
    "<html>\n<body onload='init();'>\n"
    "<script type='text/javascript'>\n"
    "function init()\n"
    "{\n"
    "setTimeout(refresh_image, 5000);\n"
    "}\n"
    "function refresh_image()\n"
    "{\n"
    "console.log('refresh_image()');\n"
    "var atari = document.getElementById('atari');\n"
    "if (atari.complete)\n"
    "{\n"
    "atari.src = 'image.gif?' + new Date().getTime();\n"
    "setTimeout(refresh_image, 33);\n"
    "}\n"
    "}\n"
    "</script>\n"
    "<table width=100%% height=100%%>"
    "<tr><td width=100%% height=100%% align='center'>"
    //"<img src='image.gif' id='atari' onload='setTimeout(refresh_image, 1000);'></td></tr>"
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

