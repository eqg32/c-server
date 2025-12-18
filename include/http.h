#ifndef HTTP
#define HTTP

#include "list.h"
#define SMALL_BUFFER_SIZE 128
#define BUFFER_SIZE 1024
#define CONNECTION_TYPE tls_connection_t *

enum response_type
{
  File,
  String
};

typedef struct request
{
  char *method;
  char *route;
  char *host;

  void  (*from_string) (struct request *self, const char *string);
} request_t;

typedef struct response
{
  int status;
  int content_length;
  enum response_type response_type;
  char *message;
  char *mime_type;

  char *string;
  char *filename;

  char *(*get_message)   (struct response *self);

  char *(*file_mime)     (const char *filename);
  int   (*file_length)   (const char *filename);
  char *(*string_mime)   (const char *string);
  int   (*string_length) (const char *string);
} response_t;

typedef struct connection
{
  int client_sock;
  int buffer_size;

  void (*read_request)  (struct connection *self, request_t *request);
  void (*send_response) (struct connection *self, const response_t *response);
  void (*shutdown)      (struct connection *self);
  void (*close)         (struct connection *self);
} connection_t;

typedef struct tls_connection
{
  struct tls *client_tls;
  connection_t *connection;

  void (*read_request)  (struct tls_connection *self, request_t *request);
  void (*send_response) (struct tls_connection *self, const response_t *response);
  void (*shutdown)      (struct tls_connection *self);
  void (*close)         (struct tls_connection *self);
} tls_connection_t;

typedef struct dispatcher
{
  list_t *handlers;

  void (*register_handler) (struct dispatcher *self, const char *route, void (*handler) (void *connection));
  void (*handle)           (const struct dispatcher *self, void *connection, request_t *request);
} dispatcher_t;

void request_init (request_t *request, const char *method, const char *path);
void request_inits (request_t *request, const char *string);
void request_free (request_t *request);

void response_initf (response_t *response, int status, const char *filename);
void response_inits (response_t *response, int status, const char *string);
void response_free  (response_t *response);

void connection_init (connection_t *connection, int client_sock, int buffer_size);
void connection_free (connection_t *connection);

void tls_connection_init (tls_connection_t *tls_connection, struct tls *ctx, connection_t *connection);
void tls_connection_free (tls_connection_t *tls_connection);

void dispatcher_init (dispatcher_t *dispatcher);
void dispatcher_free (dispatcher_t *dispatcher);

#endif
