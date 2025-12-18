#include "../include/handlers.h"
#include "../include/http.h"

void
root (void *connection)
{
  response_t r;
  CONNECTION_TYPE con = (CONNECTION_TYPE)connection;
  response_initf (&r, 200, "public/index.html");
  con->send_response (con, &r);
}

void
mountains (void *connection)
{
  response_t r;
  CONNECTION_TYPE con = (CONNECTION_TYPE)connection;
  response_initf (&r, 200, "public/mountains.jpg");
  con->send_response (con, &r);
}

void
favicon (void *connection)
{
  response_t r;
  CONNECTION_TYPE con = (CONNECTION_TYPE)connection;
  response_initf (&r, 200, "public/mountains.jpg");
  con->send_response (con, &r);
}

void
root127 (void *connection)
{
  response_t r;
  CONNECTION_TYPE con = (CONNECTION_TYPE)connection;
  response_inits (&r, 200, "just for the showcase");
  con->send_response (con, &r);
}
