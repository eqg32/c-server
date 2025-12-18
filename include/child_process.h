#ifndef CHILD_PROCESS
#define CHILD_PROCESS

#include "http.h"
#include "list.h"
#define BUFSIZE 1024
#define MAXPROC 8

void child (void *connection, list_t *dispatchers, dispatcher_t *fallback);

#endif
