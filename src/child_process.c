#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/child_process.h"
#include "../include/http.h"

void
child (connection_t *connection, const dispatcher_t *dispatcher)
{
  wait (NULL);

  pid_t pid = fork ();

  if (pid < 0)
    {
      perror ("fork");
      exit (1);
    }
  else if (pid == 0)
    {
      request_t r;
      connection->read_request (connection, &r);
      dispatcher->handle (dispatcher, connection, &r);
      connection->shutdown (connection);
      exit (0);
    }
}
