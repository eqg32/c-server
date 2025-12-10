#ifndef LIST
#define LIST

struct node
{
  char *name;
  void *value;
  struct node *next;
};

typedef struct list
{
  struct node *head;
  void (*insert) (struct list *self, const char *name, void *value);
  void (*remove) (struct list *self, const char *name);
  void * (*search) (struct list *self, const char *name);
} list_t;

void list_init (list_t *self);
void list_free (list_t *self);
void list_freec (list_t *self, void (*free_func) (void *ptr));

#endif
