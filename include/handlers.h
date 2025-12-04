#ifndef HANDLERS
#define HANDLERS

#include "child_process.h"
#include "http.h"

void root (int client_sock);
void mountains (int client_sock);
void favicon (int client_sock);

#endif
