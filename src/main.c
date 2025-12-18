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

  SSL_library_init ();
  SSL_load_error_strings ();
  SSL_CTX *server_ctx = SSL_CTX_new (TLS_server_method ());
  SSL *client_ssl;
  if (!server_ctx)
    {
      perror ("unable to create SSL context");
      exit (1);
    }

  SSL_CTX_use_certificate_file (server_ctx, config.tls_certificate,
                                SSL_FILETYPE_PEM);
  SSL_CTX_use_PrivateKey_file (server_ctx, config.tls_key, SSL_FILETYPE_PEM);

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

  list_t *dispatchers;
  list_t *handlers;
  allocptrt (dispatchers, list_t);
  allocptrt (handlers, list_t);
  list_init (dispatchers);
  list_init (handlers);

  handlers->insert (handlers, string (root), root);
  handlers->insert (handlers, string (mountains), mountains);
  handlers->insert (handlers, string (root127), root127);

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
      child (&ssl_con, dispatchers);
    }
  close (serv_sock);
  return 0;
}
