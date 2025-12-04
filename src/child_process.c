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
child (int client_sock, const dispatcher_t *dispatcher)
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
      char *route = http_get_route (client_sock, BUFSIZE);
      dispatcher_handle_request (dispatcher, client_sock, route);
      http_close_connection (client_sock);
      exit (0);
    }
}
