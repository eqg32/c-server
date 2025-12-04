#include "../include/http.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

char *
http_define_mime (const char *filename)
{
  char *mime;
  if (strstr (filename, ".html"))
    asprintf (&mime, "text/html");
  else if (strstr (filename, ".png"))
    asprintf (&mime, "image/png");
  else if (strstr (filename, ".jpg"))
    asprintf (&mime, "image/jpeg");
  else if (strstr (filename, ".tar"))
    asprintf (&mime, "application/x-tar");
  else
    asprintf (&mime, "text/plain");
  return mime;
}

char *
http_generate_headers (const char *filename)
{
  struct stat stat;
  char *headers;
  char *mime = http_define_mime (filename);
  int fd = open (filename, O_RDONLY);
  fstat (fd, &stat);
  asprintf (&headers,
            "HTTP/1.1 200 OK\r\ncontent-type: %s\r\ncontent-length: "
            "%ld\r\nconnection: close\r\n\r\n",
            mime, stat.st_size);
  free (mime);
  return headers;
}

char *
http_get_route (int client_sock, int buffer_size)
{
  char *buffer = malloc (buffer_size);
  char *route = malloc (buffer_size);
  read (client_sock, buffer, buffer_size);
  sscanf (buffer, "%*s %s", route);
  free (buffer);
  return route;
}

void
http_buffered_send (int client_sock, const char *filename, int buffer_size)
{
  int bytes_read;
  char buffer[buffer_size];
  int fd = open (filename, O_RDONLY);
  while ((bytes_read = read (fd, &buffer, buffer_size)) > 0)
    send (client_sock, buffer, bytes_read, 0);
  close (fd);
}

void
http_respond_with_file (int client_sock, const char *filename, int buffer_size)
{
  char *headers = http_generate_headers (filename);
  send (client_sock, headers, strlen (headers), 0);
  http_buffered_send (client_sock, filename, buffer_size);
  free (headers);
}

void
dispatcher_register_handler (dispatcher_t *dispatcher, const char *route,
                             void (*handler) (int client_sock))
{
  dispatcher->handlers = list_insert (dispatcher->handlers, route, handler);
}

void
dispatcher_handle_request (const dispatcher_t *dispatcher, int client_sock,
                           const char *route)
{
  list_t *node = list_search (dispatcher->handlers, route);
  if (!node)
    {
      char buffer[] = "HTTP/1.1 403 Forbidden\r\n\r\nUser is not allowed to "
                      "make such requests.\r\n";
      send (client_sock, buffer, strlen (buffer), 0);
      shutdown (client_sock, SHUT_WR);
      close (client_sock);
    }
  else
    node->handler (client_sock);
}
