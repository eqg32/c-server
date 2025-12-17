#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

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

  SSL_library_init ();
  SSL_load_error_strings ();
  SSL_CTX *server_ctx = SSL_CTX_new (TLS_server_method ());
  SSL *client_ssl;
  if (!server_ctx)
    {
      perror ("unable to create SSL context");
      exit (1);
    }

  SSL_CTX_use_certificate_file (server_ctx, "private/server.pem",
                                SSL_FILETYPE_PEM);
  SSL_CTX_use_PrivateKey_file (server_ctx, "private/privkey.pem",
                               SSL_FILETYPE_PEM);

  SSL_CTX_set_options (server_ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
  SSL_CTX_set_cipher_list (server_ctx, "HIGH:!aNULL:!MD5:!RC4");
  if (SSL_CTX_check_private_key (server_ctx) == -1)
    {
      perror ("check");
      exit (1);
    }

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
  while (1)
    {
      if ((client_sock
           = accept (serv_sock, (struct sockaddr *)&sockaddr, &socklen))
          < 0)
        {
          perror ("accept");
          exit (1);
        }

      client_ssl = SSL_new (server_ctx);
      if (!client_ssl)
        {
          perror ("TLS handshake");
          continue;
        }

      SSL_set_fd (client_ssl, client_sock);
      SSL_accept (client_ssl);

      connection_t con;
      ssl_connection_t ssl_con;

      connection_init (&con, client_sock, 1024);
      ssl_connection_init (&ssl_con, client_ssl, &con);
      child (&ssl_con, &d);
    }
  close (serv_sock);
  return 0;
}
