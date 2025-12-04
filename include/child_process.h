#ifndef CHILD_PROCESS
#define CHILD_PROCESS

#include "http.h"
#define BUFSIZE 1024
#define MAXPROC 8

void child (int client_sock, const dispatcher_t *dispatcher);

#endif
