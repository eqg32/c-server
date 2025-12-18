#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/child_process.h"
#include "../include/http.h"

void
child (void *connection, list_t *dispatchers)
{
  wait (NULL);

  pid_t pid = fork ();
  CONNECTION_TYPE con = (CONNECTION_TYPE)connection;

  if (pid < 0)
    {
      perror ("fork");
      exit (1);
    }
  else if (pid == 0)
    {
      request_t r;
      con->read_request (con, &r);
      dispatcher_t *d
          = (dispatcher_t *)dispatchers->search (dispatchers, r.host);
      d->handle (d, con, &r);
      con->shutdown (con);
      con->close (con);
      exit (0);
    }
  else if (pid > 0)
    {
      con->close (con);
    }
}
