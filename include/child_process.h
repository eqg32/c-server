#ifndef CHILD_PROCESS
#define CHILD_PROCESS

#include "http.h"
#define BUFSIZE 1024
#define MAXPROC 8

void child (void *connection, const dispatcher_t *dispatcher);

#endif
