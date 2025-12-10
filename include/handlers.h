#ifndef HANDLERS
#define HANDLERS

#include "child_process.h"
#include "http.h"

void root (connection_t *connection);
void mountains (connection_t *connection);
void favicon (connection_t *connection);

#endif
