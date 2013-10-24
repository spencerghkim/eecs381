#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "p1_globals.h"
#include "Ordered_container.h"
#include "Utility.h"

typedef int (*OC_comp_func)(const void*, const void*);

/* struct LL_Node structure declaration. This declaration is local to this file. 
This is a two-way or doubly-linked list. Each node has a pointer to the previous 
node and another pointer to the next node in the list. This means insertions or
removals of a node can be made in constant time, once the location has been
determined. */
struct LL_Node { 
    struct LL_Node* prev;      /* pointer to the previous node */
	struct LL_Node* next;		/* pointer to the next node */
	void* data_ptr; 			/* uncommitted pointer to the data item */
};

/* Declaration for Ordered_container. This declaration is local to this file.  
A pointer is maintained to the last node in the list as well as the first,
meaning that additions to the end of the list can be made in constant time. 
The number of nodes in the list is kept up-to-date in the size member
variable, so that the size of the list can be accessed in constant time. */
struct Ordered_container {
	OC_comp_fp_t comp_func;
	struct LL_Node* first;
	struct LL_Node* last;
	int size;
};

/* Helper function for searching for a node. If a node containing a data pointer
 * that compares equal to the provided 'data_ptr' exists, we return true
 * and provide the caller with a pointer to the node, via the 'node_ptr' param.
 * If it does not exist, we return false and 'node_ptr' points to the node
 * that resides immediately after the position where the searched-for node
 * should be inserted (or NULL if it belongs at the tail of the list). */
static int find_node_or_next(const struct Ordered_container* c_ptr,
                             const void* data_ptr,
                             OC_comp_func,
                             struct LL_Node** node_ptr);

struct Ordered_container* OC_create_container(OC_comp_fp_t comp_func)
{
  struct Ordered_container* container =
      safe_malloc(sizeof(struct Ordered_container));

  /* Initialize container variables. */
  container->comp_func = comp_func;
  container->first = NULL;
  container->last = NULL;
  container->size = 0;

  g_Container_count++;

  return container;
}

void OC_destroy_container(struct Ordered_container* c_ptr)
{
  OC_clear(c_ptr); 

  /* After freeing all list elements, free the container. */
  free(c_ptr);

  assert(g_Container_count > 0);
  g_Container_count--;
}

void OC_clear(struct Ordered_container* c_ptr)
{
  struct LL_Node* temp_node_ptr;
  struct LL_Node* cur_node_ptr = c_ptr->first;

  while (cur_node_ptr) {
    /* Hold on to the node ptr so we can free it after grabbing next. */
    temp_node_ptr = cur_node_ptr;
    cur_node_ptr = cur_node_ptr->next;

    free(temp_node_ptr); 
  }

  /* Update the globals. */
  g_Container_items_in_use -= c_ptr->size;
  g_Container_items_allocated -= c_ptr->size;

  /* Fix the Ordered_container member variables. */
  c_ptr->first = NULL;
  c_ptr->last = NULL;
  c_ptr->size = 0;
}

int OC_get_size(const struct Ordered_container* c_ptr)
{
  return c_ptr->size;
}

int OC_empty(const struct Ordered_container* c_ptr)
{
  return c_ptr->size == 0;
}

void* OC_get_data_ptr(const void* item_ptr)
{
  return ((struct LL_Node*)(item_ptr))->data_ptr;
}

void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr)
{
  struct LL_Node* node_ptr = (struct LL_Node*)(item_ptr);

  /* Fix previous element in the list, or list head if previous element DNE. */
  if (node_ptr->prev) {
    node_ptr->prev->next = node_ptr->next;
  } else {
    c_ptr->first = node_ptr->next;
  }

  /* Fix next element in the list, or list tail if next element DNE. */
  if (node_ptr->next) {
    node_ptr->next->prev = node_ptr->prev;
  } else {
    c_ptr->last = node_ptr->prev;
  }

  /* Now that we've fixed the other nodes, delete the item. */
  free(node_ptr);
  c_ptr->size--;

  g_Container_items_in_use--;
  g_Container_items_allocated--;
}

void OC_insert(struct Ordered_container* c_ptr, void* data_ptr)
{
  struct LL_Node* new_node_ptr = safe_malloc(sizeof(struct LL_Node));
  struct LL_Node* closest_node_ptr = NULL;
  
  /* Find where to insert the new node. Ignore the return value - we don't
   * care if a node already exists with the same value. */
  find_node_or_next(c_ptr, data_ptr, c_ptr->comp_func, &closest_node_ptr);

  /* Initialize the new item ptr's fields. */
  new_node_ptr->data_ptr = data_ptr;
  new_node_ptr->next = closest_node_ptr;
  new_node_ptr->prev =
      (closest_node_ptr) ? closest_node_ptr->prev : c_ptr->last;

  /* Fix previous item in the list, or the list head if previous element DNE. */
  if (new_node_ptr->prev) {
    new_node_ptr->prev->next = new_node_ptr;
  } else {
    c_ptr->first = new_node_ptr;
  }

  /* Fix next item in the list, or the list tail if next element DNE. */
  if (new_node_ptr->next) {
    new_node_ptr->next->prev = new_node_ptr;
  } else {
    c_ptr->last = new_node_ptr;
  }

  c_ptr->size++;
  
  g_Container_items_in_use++;
  g_Container_items_allocated++;
}

void* OC_find_item(const struct Ordered_container* c_ptr, const void* data_ptr)
{
  struct LL_Node* node_ptr = NULL;
  
  int found = find_node_or_next(c_ptr, data_ptr, c_ptr->comp_func, &node_ptr);

  return (found) ? node_ptr : NULL;
}

void* OC_find_item_arg(const struct Ordered_container* c_ptr,
                       const void* arg_ptr, OC_find_item_arg_fp_t fafp)
{
  struct LL_Node* node_ptr = NULL;

  int found = find_node_or_next(c_ptr, arg_ptr, fafp, &node_ptr);

  return (found) ? node_ptr : NULL;
}

void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp)
{
  struct LL_Node* node_ptr;

  for (node_ptr = c_ptr->first; node_ptr; node_ptr = node_ptr->next) {
    afp(node_ptr->data_ptr);
  }
}

int OC_apply_if(const struct Ordered_container* c_ptr, OC_apply_if_fp_t afp)
{
  struct LL_Node* node_ptr;
  int return_val;

  for (node_ptr = c_ptr->first, return_val = 0;
       node_ptr && (return_val = afp(node_ptr->data_ptr)) == 0;
       node_ptr = node_ptr->next);

  return return_val;
}

void OC_apply_arg(const struct Ordered_container* c_ptr,
                  OC_apply_arg_fp_t afp,
                  void* arg_ptr)
{
  struct LL_Node* node_ptr;

  for (node_ptr = c_ptr->first; node_ptr; node_ptr = node_ptr->next) {
    afp(node_ptr->data_ptr, arg_ptr);
  }
}

int OC_apply_if_arg(const struct Ordered_container* c_ptr,
                    OC_apply_if_arg_fp_t afp,
                    void* arg_ptr)
{
  struct LL_Node* node_ptr;
  int return_val;

  for (node_ptr = c_ptr->first, return_val = 0;
       node_ptr && (return_val = afp(node_ptr->data_ptr, arg_ptr)) == 0;
       node_ptr = node_ptr->next);

  return return_val;
}

static int find_node_or_next(const struct Ordered_container* c_ptr,
                             const void* data_ptr,
                             OC_comp_func comp_func,
                             struct LL_Node** node_ptr)
{
  int result;

  for (*node_ptr = c_ptr->first, result = -1;
       *node_ptr && (result = comp_func(data_ptr, (*node_ptr)->data_ptr)) > 0;
       *node_ptr = (*node_ptr)->next);
  
  return (result == 0);
}

