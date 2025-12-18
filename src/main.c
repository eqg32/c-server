#include <fcntl.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <tls.h>

#include "../include/child_process.h"
#include "../include/config.h"
#include "../include/handlers.h"
#include "../include/utils.h"

#define string(arg) #arg

int
main (int argc, char *argv[])
{
  config_t config;
  allocptrt (config.dispatchers, list_t);
  config_init (&config, "config");

  int port = 8080;
  if (argc >= 2)
    {
      port = atoi (argv[1]);
    }
  int port_ns = htons (port);

  struct tls *server_ctx;
  struct tls *client_ctx;
  struct tls_config *tls_config;

  /* tls configuration */
  tls_config = tls_config_new ();
  server_ctx = tls_server ();
  tls_config_set_cert_file (tls_config, config.tls_certificate);
  tls_config_set_key_file (tls_config, config.tls_key);
  tls_configure (server_ctx, tls_config);

  /* socket related variables */
  int serv_sock = socket (AF_INET, SOCK_STREAM, 0), client_sock;
  if (serv_sock == 0)
    {
      perror ("socket");
      exit (1);
    }
  struct sockaddr_in sockaddr
      = { AF_INET, port_ns, .sin_addr.s_addr = INADDR_ANY };
  socklen_t socklen = sizeof (sockaddr);

  if ((bind (serv_sock, (struct sockaddr *)&sockaddr, socklen)) < 0)
    {
      perror ("bind");
      exit (1);
    }

  if ((listen (serv_sock, MAXPROC)) < 0)
    {
      perror ("listen");
      exit (1);
    }

  /* these processes just kill themselves */
  for (int i = 0; i < MAXPROC; i++)
    {
      pid_t pid = fork ();
      if (pid < 0)
        {
          perror ("fork");
        }
      else if (pid == 0)
        {
          exit (0);
        }
    }

  list_t *dispatchers;
  list_t *handlers;
  allocptrt (dispatchers, list_t);
  allocptrt (handlers, list_t);
  list_init (dispatchers);
  list_init (handlers);

  handlers->insert (handlers, string (root), root);
  handlers->insert (handlers, string (mountains), mountains);

  struct node *tmp = config.dispatchers->head;

  while (tmp)
    {
      size_t size, read;
      char route[SMALL_BUFFER_SIZE], handler[SMALL_BUFFER_SIZE];
      char *filename, *buffer;

      dispatcher_t *d;
      allocptrt (d, dispatcher_t);
      allocptrt (d->handlers, list_t);
      dispatcher_init (d);

      asprintf (&filename, "disp.%s", tmp->name);
      FILE *file = fopen (filename, "r");

      while ((read = getline (&buffer, &size, file)) != -1)
        {
          sscanf (buffer, "%s %s", route, handler);
          d->register_handler (d, route, handlers->search (handlers, handler));
        }

      dispatchers->insert (dispatchers, tmp->name, d);
      fclose (file);
      tmp = tmp->next;
    }

  /* fork all the connections and process them */
  while (1)
    {
      if ((client_sock
           = accept (serv_sock, (struct sockaddr *)&sockaddr, &socklen))
          < 0)
        {
          perror ("accept");
          exit (1);
        }
      if (tls_accept_socket (server_ctx, &client_ctx, client_sock) == -1)
        {
          perror ("tls accept");
          exit (1);
        }
      connection_t con;
      tls_connection_t tls_con;
      connection_init (&con, client_sock, 1024);
      tls_connection_init (&tls_con, client_ctx, &con);
      child (&tls_con, dispatchers);
    }
  close (serv_sock);
  return 0;
}
