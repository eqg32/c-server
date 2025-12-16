#include "../include/handlers.h"
#include "../include/http.h"

void
root (void *connection)
{
  response_t r;
  tls_connection_t *con = (tls_connection_t *)connection;
  response_initf (&r, 200, "public/index.html");
  con->send_response (con, &r);
}

void
mountains (void *connection)
{
  response_t r;
  tls_connection_t *con = (tls_connection_t *)connection;
  response_initf (&r, 200, "public/mountains.jpg");
  con->send_response (con, &r);
}

void
favicon (void *connection)
{
  response_t r;
  tls_connection_t *con = (tls_connection_t *)connection;
  response_initf (&r, 200, "public/mountains.jpg");
  con->send_response (con, &r);
}
