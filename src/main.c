#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <tls.h>

#include "../include/child_process.h"
#include "../include/handlers.h"

int
main (int argc, char *argv[])
{
  int port = 8080;
  if (argc >= 2)
    {
      port = atoi (argv[1]);
    }
  int port_ns = htons (port);

  struct tls *server_ctx;
  struct tls *client_ctx;
  struct tls_config *config;

  /* tls configuration */
  config = tls_config_new ();
  server_ctx = tls_server ();
  tls_config_set_cert_file (config, "private/server.crt");
  tls_config_set_key_file (config, "private/server.key");
  tls_configure (server_ctx, config);

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

  dispatcher_t d;
  d.handlers = malloc (sizeof (list_t));
  dispatcher_init (&d);
  d.register_handler (&d, "/", root);
  d.register_handler (&d, "/mountains.jpg", mountains);
  d.register_handler (&d, "/favicon.ico", favicon);

  /* fork all the connections and process them */
  for (;;)
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
      child (&tls_con, &d);
    }
  close (serv_sock);
  tls_free (server_ctx);
  tls_config_free (config);
  return 0;
}
