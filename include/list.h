#ifndef LIST
#define LIST

typedef struct node
{
  char *name;
  void *value;
  struct node *next;
} list_t;

list_t *list_insert (list_t *head, const char *name, void *value);
list_t *list_delete (list_t *head, const char *name);
list_t *list_search (list_t *head, const char *name);

#endif
