#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/child_process.h"
#include "../include/http.h"

void
child (void *connection, const dispatcher_t *dispatcher)
{
  wait (NULL);

  pid_t pid = fork ();
  tls_connection_t *con = (tls_connection_t *)connection;

  if (pid < 0)
    {
      perror ("fork");
      exit (1);
    }
  else if (pid == 0)
    {
      request_t r;
      con->read_request (con, &r);
      dispatcher->handle (dispatcher, con, &r);
      con->shutdown (con);
      con->close (con);
      exit (0);
    }
  else if (pid > 0)
    {
      con->close (con);
    }
}
