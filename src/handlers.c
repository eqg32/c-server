#include "../include/handlers.h"
#include "../include/http.h"

void
root (connection_t *connection)
{
  response_t r;
  response_initf (&r, 200, "public/index.html");
  connection->send_response (connection, &r);
}

void
mountains (connection_t *connection)
{
  response_t r;
  response_initf (&r, 200, "public/mountains.jpg");
  connection->send_response (connection, &r);
}

void
favicon (connection_t *connection)
{
  response_t r;
  response_initf (&r, 200, "public/mountains.jpg");
  connection->send_response (connection, &r);
}
