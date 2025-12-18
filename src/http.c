#include "../include/http.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <tls.h>
#include <unistd.h>

void
request_from_string (request_t *self, const char *string)
{
  char method[SMALL_BUFFER_SIZE];
  char route[SMALL_BUFFER_SIZE];
  char host[SMALL_BUFFER_SIZE];
  sscanf (string, "%s %s", method, route);
  asprintf (&self->method, "%s", method);
  asprintf (&self->route, "%s", route);
  string = strstr (string, "ost: ");
  sscanf (string, "%*s %s", host);
  *(strstr (host, ":")) = '\0';
  asprintf (&self->host, "%s", host);
}

void
request_init (request_t *request, const char *method, const char *path)
{
  asprintf (&request->method, "%s", method);
  asprintf (&request->route, "%s", path);
  request->from_string = request_from_string;
}

void
request_inits (request_t *request, const char *string)
{
  request->from_string = request_from_string;
  request->from_string (request, string);
}

void
request_free (request_t *request)
{
  free (request->method);
  free (request->route);
  free (request);
}

char *
response_get_message (response_t *self)
{
  switch (self->status)
    {
    case 200:
      return "OK";
      break;
    case 403:
      return "Forbidden";
      break;
    }
  self->status = 400;
  return "Bad Request";
}

char *
response_file_mime (const char *filename)
{
  if (strstr (filename, ".html"))
    return "text/html";
  else if (strstr (filename, ".png"))
    return "image/png";
  else if (strstr (filename, ".jpg"))
    return "image/jpeg";
  else if (strstr (filename, ".tar"))
    return "application/x-tar";
  else
    return "text/html";
}

char *
response_string_mime (const char *string)
{
  return "text/html";
}

int
response_file_length (const char *filename)
{
  struct stat stat;
  int fd = open (filename, O_RDONLY);
  char *length;
  fstat (fd, &stat);
  close (fd);
  return stat.st_size;
}

int
response_string_length (const char *string)
{
  return strlen (string);
}

void
response_use_file (response_t *self, const char *filename)
{
  asprintf (&self->filename, "%s", filename);
  self->response_type = File;
  self->mime_type = self->file_mime (filename);
  self->content_length = self->file_length (filename);
  self->message = self->get_message (self);
}

void
response_use_string (response_t *self, const char *string)
{
  asprintf (&self->string, "%s", string);
  self->response_type = String;
  self->mime_type = self->string_mime (string);
  self->content_length = self->string_length (string);
  self->message = self->get_message (self);
}

void
response_initf (response_t *response, int status, const char *filename)
{
  response->status = status;
  response->get_message = response_get_message;
  response->file_mime = response_file_mime;
  response->file_length = response_file_length;

  asprintf (&response->filename, "%s", filename);
  response->response_type = File;
  response->mime_type = response->file_mime (filename);
  response->content_length = response->file_length (filename);
  response->message = response->get_message (response);
}

void
response_inits (response_t *response, int status, const char *string)
{
  response->status = status;
  response->get_message = response_get_message;
  response->string_mime = response_string_mime;
  response->string_length = response_string_length;

  asprintf (&response->string, "%s", string);
  response->response_type = String;
  response->mime_type = response->string_mime (string);
  response->content_length = response->string_length (string);
  response->message = response->get_message (response);
}

void
response_free (response_t *response)
{
  free (response->message);
  free (response->mime_type);
  if (response->response_type == File)
    free (response->filename);
  else if (response->response_type == String)
    free (response->string);
  free (response);
}

void
connection_read_request (connection_t *self, request_t *request)
{
  char buffer[self->buffer_size];
  read (self->client_sock, buffer, self->buffer_size);
  request_inits (request, buffer);
}

void
connection_send_response (connection_t *self, const response_t *response)
{
  int fd;
  int bytes_read;
  char buffer[self->buffer_size];
  char *headers = NULL;

  asprintf (&headers,
            "HTTP/1.1 %d %s\r\ncontent-length: %d\r\ncontent-type: %s\r\n\r\n",
            response->status, response->message, response->content_length,
            response->mime_type);
  write (self->client_sock, headers, strlen (headers));
  switch (response->response_type)
    {
    case String:
      write (self->client_sock, response->string, strlen (response->string));
      break;
    case File:
      fd = open (response->filename, O_RDONLY);
      while ((bytes_read = read (fd, buffer, self->buffer_size)) != 0)
        write (self->client_sock, buffer, bytes_read);
      close (fd);
      break;
    }
  free (headers);
}

void
connection_shutdown (connection_t *self)
{
  shutdown (self->client_sock, SHUT_WR);
}

void
connection_close (connection_t *self)
{
  close (self->client_sock);
}

void
connection_init (connection_t *connection, int client_sock, int buffer_size)
{
  connection->client_sock = client_sock;
  connection->buffer_size = buffer_size;
  connection->read_request = connection_read_request;
  connection->send_response = connection_send_response;
  connection->shutdown = connection_shutdown;
  connection->close = connection_close;
}

void
connection_free (connection_t *connection)
{
  connection->shutdown (connection);
  free (connection);
}

void
tls_connection_read_request (tls_connection_t *self, request_t *request)
{
  char buffer[self->connection->buffer_size];
  tls_read (self->client_tls, buffer, self->connection->buffer_size);
  request_inits (request, buffer);
}

void
tls_connection_send_response (tls_connection_t *self,
                              const response_t *response)
{
  int fd;
  int bytes_read;
  char buffer[self->connection->buffer_size];
  char *headers = NULL;

  asprintf (&headers,
            "HTTP/1.1 %d %s\r\ncontent-length: %d\r\ncontent-type: %s\r\n\r\n",
            response->status, response->message, response->content_length,
            response->mime_type);
  tls_write (self->client_tls, headers, strlen (headers));
  switch (response->response_type)
    {
    case String:
      tls_write (self->client_tls, response->string,
                 strlen (response->string));
      break;
    case File:
      fd = open (response->filename, O_RDONLY);
      while ((bytes_read = read (fd, buffer, self->connection->buffer_size))
             != 0)
        tls_write (self->client_tls, buffer, bytes_read);
      close (fd);
      break;
    }
  free (headers);
}

void
tls_connection_shutdown (tls_connection_t *self)
{
  tls_close (self->client_tls);
  tls_free (self->client_tls);
}

void
tls_connection_close (tls_connection_t *self)
{
  self->connection->close (self->connection);
}

void
tls_connection_init (tls_connection_t *tls_connection, struct tls *ctx,
                     connection_t *connection)
{
  tls_connection->client_tls = ctx;
  tls_connection->connection = connection;
  tls_connection->read_request = tls_connection_read_request;
  tls_connection->send_response = tls_connection_send_response;
  tls_connection->shutdown = tls_connection_shutdown;
  tls_connection->close = tls_connection_close;
}

void
tls_connection_free (tls_connection_t *tls_connection)
{
  tls_connection->shutdown (tls_connection);
  free (tls_connection);
}

void
dispatcher_register_handler (dispatcher_t *self, const char *route,
                             void (*handler) (void *connection))
{
  self->handlers->insert (self->handlers, route, handler);
}

void
dispatcher_handle (const dispatcher_t *self, void *connection,
                   request_t *request)
{
  CONNECTION_TYPE con = (CONNECTION_TYPE)connection;
  void (*handler) (void *)
      = self->handlers->search (self->handlers, request->route);
  if (!handler)
    {
      response_t r;
      response_inits (
          &r, 403,
          "<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\"> "
          "<title>403 Forbidden</title> </head> <body> <center> <h1>403 "
          "Forbidden</h1> <hr width=\"50%\"> <p>You don't have permission to "
          "access this page.</p> </center> </body> </html>");
      con->send_response (con, &r);
    }
  else
    handler (con);
}

void
dispatcher_init (dispatcher_t *dispatcher)
{
  dispatcher->register_handler = dispatcher_register_handler;
  dispatcher->handle = dispatcher_handle;
  list_init (dispatcher->handlers);
}

void
dispatcher_free (dispatcher_t *dispatcher)
{
  list_free (dispatcher->handlers);
  free (dispatcher);
}
