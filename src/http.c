#include "../include/http.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

pair_t *
pair_create (const char *key, const char *value)
{
  pair_t *new = malloc (sizeof (pair_t));
  new->pair[0] = malloc (strlen (key));
  new->pair[1] = malloc (strlen (value));
  strcpy (new->pair[0], key);
  strcpy (new->pair[1], value);
  return new;
}

char *
pair_to_string (const pair_t *pair)
{
  char *h;
  asprintf (&h, "%s: %s\r\n", pair->pair[0], pair->pair[1]);
  return h;
}

char *
http_get_mime (const char *filename)
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

headers_t *
headers_create (int status, const char *message, list_t *pairs)
{
  headers_t *headers = malloc (sizeof (headers_t));
  headers->status = status;
  headers->message = malloc (strlen (message));
  strcpy (headers->message, message);
  headers->pairs = pairs;
  return headers;
}

char *
headers_to_string (const headers_t *headers)
{
  char *h = malloc (1024);
  list_t *tmp = headers->pairs;
  asprintf (&h, "HTTP/1.1 %d %s\r\n", headers->status, headers->message);
  while (tmp)
    {
      h = strcat (h, pair_to_string ((pair_t *)tmp->value));
      tmp = tmp->next;
    }
  h = strcat (h, "\r\n");
  list_destroy (headers->pairs);
  free ((void *)headers);
  return h;
}

char *
http_get_file_length (const char *filename)
{
  struct stat stat;
  int fd = open (filename, O_RDONLY);
  char *length;
  fstat (fd, &stat);
  close (fd);
  asprintf (&length, "%ld", stat.st_size);
  return length;
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

char *
http_generate_headers_string (int status, const char *message,
                              const char *string)
{
  char *length;
  asprintf (&length, "%lu", strlen (string));
  list_t *pairs = NULL;
  pairs = list_insert (pairs, "ct", pair_create ("content-type", "text/html"));
  pairs = list_insert (pairs, "cl", pair_create ("content-length", length));
  free (length);
  headers_t *headers = headers_create (status, message, pairs);
  return headers_to_string (headers);
}

char *
http_generate_headers_file (int status, const char *message,
                            const char *filename)
{
  list_t *pairs = NULL;
  pairs = list_insert (pairs, "ct", pair_create ("content-type", "text/html"));
  pairs = list_insert (
      pairs, "cl",
      pair_create ("content-length", http_get_file_length (filename)));
  headers_t *headers = headers_create (status, message, pairs);
  return headers_to_string (headers);
}

void
http_close_connection (int client_sock)
{
  shutdown (client_sock, SHUT_WR);
  close (client_sock);
}

void
http_send_buffered (int client_sock, const char *filename, int buffer_size)
{
  int bytes_read;
  char buffer[buffer_size];
  int fd = open (filename, O_RDONLY);
  while ((bytes_read = read (fd, &buffer, buffer_size)) > 0)
    send (client_sock, buffer, bytes_read, 0);
  close (fd);
}

void
http_respond_with_string (const response_t *response)
{
  char *hs = http_generate_headers_string (response->status, response->message,
                                           response->string);
  send (response->client_sock, hs, strlen (hs), 0);
  send (response->client_sock, response->string, strlen (response->string), 0);
  free (hs);
}

void
http_respond_with_file (const response_t *response)
{
  char *hs = http_generate_headers_file (response->status, response->message,
                                         response->filename);
  send (response->client_sock, hs, strlen (hs), 0);
  http_send_buffered (response->client_sock, response->filename,
                      response->buffer_size);
  free (hs);
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
      http_close_connection (client_sock);
    }
  else
    ((void (*) (int)) (node->value)) (client_sock);
}
