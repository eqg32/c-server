#ifndef HTTP
#define HTTP

#include <stdlib.h>
#include "list.h"

typedef struct dispatcher
{
  list_t *handlers;
} dispatcher_t;

typedef struct pair
{
  char *pair[2];
} pair_t;

typedef struct headers
{
  int status;
  char *message;
  list_t *pairs;
} headers_t;

typedef struct response
{
  int client_sock;
  int status;
  int buffer_size;
  char *message;
  char *string;
  char *filename;
} response_t;

char *http_get_mime (const char *filename);
char *http_get_file_length (const char *filename);
char *http_get_route (int client_sock, int buffer_size);

char *http_generate_headers_string (int status, const char *message, const char *string);
char *http_generate_headers_file (int status, const char *message, const char *filename);

void http_close_connection (int client_sock);
void http_send_buffered (int client_sock, const char *filename, int buffer_size);

void http_respond_with_string (const response_t *response);
void http_respond_with_file (const response_t *respone);

void dispatcher_register_handler (dispatcher_t *dispatcher, const char *route, void (*handler) (int client_sock));
void dispatcher_handle_request (const dispatcher_t *dispatcher, int client_sock, const char *route);

#endif
