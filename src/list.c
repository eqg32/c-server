#include "../include/list.h"
#include <stdlib.h>
#include <string.h>

list_t *
list_insert (list_t *head, const char *name, void *value)
{
  if (!head)
    {
      list_t *new = (list_t *)malloc (sizeof (list_t));
      new->value = value;
      new->name = malloc (strlen (name));
      strcpy (new->name, name);
      new->next = NULL;
      return new;
    }
  head->next = list_insert (head->next, name, value);
  return head;
}

list_t *
list_delete (list_t *head, const char *name)
{
  if (!head)
    {
      return head;
    }
  if (!strcmp (head->name, name))
    {
      list_t *tmp = head;
      head = head->next;
      free (tmp);
      return head;
    }
  head->next = list_delete (head->next, name);
  return head;
}

list_t *
list_search (list_t *head, const char *name)
{
  if (!head)
    return head;
  if (!strcmp (head->name, name))
    return head;
  return list_search (head->next, name);
}
