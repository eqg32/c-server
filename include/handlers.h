#ifndef HANDLERS
#define HANDLERS

#include "child_process.h"
#include "http.h"

void root (void *connection);
void mountains (void *connection);
void favicon (void *connection);

void root127 (void *connection);

#endif
