#include "../include/list.h"
#include <stdlib.h>
#include <string.h>

list_t *
list_insert (list_t *head, const char *route,
             void (*handler) (int client_sock))
{
  if (!head)
    {
      list_t *new = (list_t *)malloc (sizeof (list_t));
      new->handler = handler;
      new->route = malloc (strlen (route));
      strcpy (new->route, route);
      new->next = NULL;
      return new;
    }
  head->next = list_insert (head->next, route, handler);
  return head;
}

list_t *
list_delete (list_t *head, const char *route)
{
  if (!head)
    {
      return head;
    }
  if (!strcmp (head->route, route))
    {
      list_t *tmp = head;
      head = head->next;
      free (tmp);
      return head;
    }
  head->next = list_delete (head->next, route);
  return head;
}

list_t *
list_search (list_t *head, const char *route)
{
  if (!head)
    return head;
  if (!strcmp (head->route, route))
    return head;
  return list_search (head->next, route);
}
