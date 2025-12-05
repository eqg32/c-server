#include "../include/handlers.h"
#include "../include/http.h"

void
root (int client_sock)
{
  response_t r;
  r.client_sock = client_sock;
  r.status = 200;
  r.message = "OK";
  r.filename = "public/index.html";
  r.buffer_size = BUFSIZE;
  http_respond_with_file (&r);
}

void
mountains (int client_sock)
{
  response_t r;
  r.client_sock = client_sock;
  r.status = 200;
  r.message = "OK";
  r.filename = "public/mountains.jpg";
  r.buffer_size = BUFSIZE;
  http_respond_with_file (&r);
}

void
favicon (int client_sock)
{
  response_t r;
  r.client_sock = client_sock;
  r.status = 200;
  r.message = "OK";
  r.filename = "public/emacs.png";
  r.buffer_size = BUFSIZE;
  http_respond_with_file (&r);
}
