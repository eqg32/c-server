#ifndef LIST
#define LIST

typedef struct node
{
  char *route;
  void (*handler) (int client_sock);
  struct node *next;
} list_t;

list_t *list_insert (list_t *head, const char *route, void (*handler) (int client_sock));
list_t *list_delete (list_t *head, const char *route);
list_t *list_search (list_t *head, const char *route);

#endif
