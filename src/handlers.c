#include "../include/handlers.h"

void
root (int client_sock)
{
  http_respond_with_file (client_sock, "public/index.html", BUFSIZE);
}

void
mountains (int client_sock)
{
  http_respond_with_file (client_sock, "public/mountains.jpg", BUFSIZE);
}

void
favicon (int client_sock)
{
  http_respond_with_file (client_sock, "public/emacs.png", BUFSIZE);
}
