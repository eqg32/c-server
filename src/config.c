#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/config.h"
#include "../include/list.h"
#include "../include/utils.h"

void
config_init (config_t *config, const char *config_path)
{
  list_init (config->dispatchers);
  char *buffer, *key, *value;
  allocptr (key, 64);
  allocptr (value, 64);
  size_t size, read;
  FILE *config_file = fopen (config_path, "r");
  while ((read = getline (&buffer, &size, config_file)) != -1)
    {
      sscanf (buffer, "%s %s", key, value);
      if (!strcmp (key, "port"))
        config->port = atoi (value);
      if (!strcmp (key, "tls_certificate"))
        asprintf (&config->tls_certificate, "%s", value);
      if (!strcmp (key, "tls_key"))
        asprintf (&config->tls_key, "%s", value);
      if (!strcmp (key, "dispatcher"))
        config->dispatchers->insert (config->dispatchers, value, NULL);
    }
  fclose (config_file);
}

void
config_free (config_t *config)
{
  free (config->tls_certificate);
  free (config->tls_key);
  list_free (config->dispatchers);
  free (config);
}
