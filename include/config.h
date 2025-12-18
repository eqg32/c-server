#ifndef SERVER_CONFIG
#define SERVER_CONFIG

#include "../include/list.h"

typedef struct config
{
  int port;
  char *tls_certificate;
  char *tls_key;
  list_t *dispatchers;
} config_t;

void config_init (config_t *config, const char *config_path);
void config_free (config_t *config);

#endif
