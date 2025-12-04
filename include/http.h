#ifndef HTTP
#define HTTP

#include "list.h"
typedef struct dispatcher
{
  list_t *handlers;
} dispatcher_t;

char *http_define_mime (const char *filename);
char *http_generate_headers (const char *filename);
char *http_get_route (int client_sock, int buffer_size);
void http_buffered_send (int client_sock, const char *filename, int buffer_size);
void http_respond_with_file (int client_sock, const char *filename, int buffer_size);
void dispatcher_register_handler (dispatcher_t *dispatcher, const char *route, void (*handler) (int client_sock));
void dispatcher_handle_request (const dispatcher_t *dispatcher, int client_sock, const char *route);

#endif
