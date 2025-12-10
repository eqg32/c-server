#include "../include/list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct node *
node_insert (struct node *head, const char *name, void *value)
{
  if (!head)
    {
      struct node *new = (struct node *)malloc (sizeof (struct node));
      new->value = value;
      new->name = malloc (strlen (name));
      asprintf (&new->name, "%s", name);
      new->next = NULL;
      return new;
    }
  head->next = node_insert (head->next, name, value);
  return head;
}

struct node *
node_remove (struct node *head, const char *name)
{
  if (!head)
    {
      return head;
    }
  if (!strcmp (head->name, name))
    {
      struct node *tmp = head;
      head = head->next;
      free (tmp->name);
      free (tmp);
      return head;
    }
  head->next = node_remove (head->next, name);
  return head;
}

struct node *
node_search (struct node *head, const char *name)
{
  if (!head)
    return head;
  if (!strcmp (head->name, name))
    return head;
  return node_search (head->next, name);
}

void
node_free (struct node *head)
{
  if (head)
    {
      struct node *tmp = head->next;
      free (head->name);
      free (head);
      node_free (tmp);
    }
}

void
node_freec (struct node *head, void (*free_func) (void *ptr))
{
  if (head)
    {
      struct node *tmp = head->next;
      free (head->name);
      free_func (head->value);
      free (head);
      node_freec (tmp, free_func);
    }
}

void
list_insert (list_t *self, const char *name, void *value)
{
  self->head = node_insert (self->head, name, value);
}

void
list_remove (list_t *self, const char *name)
{
  self->head = node_remove (self->head, name);
}

void *
list_search (list_t *self, const char *name)
{
  struct node *node = node_search (self->head, name);
  if (!node)
    return NULL;
  return node->value;
}

void
list_init (list_t *list)
{
  list->head = NULL;
  list->insert = list_insert;
  list->remove = list_remove;
  list->search = list_search;
}

void
list_free (list_t *list)
{
  node_free (list->head);
  free (list);
}

void
list_freec (list_t *list, void (*free_func) (void *))
{
  node_freec (list->head, free_func);
  free (list);
}
