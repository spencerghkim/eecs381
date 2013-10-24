#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "p1_globals.h"
#include "Ordered_container.h"
#include "Utility.h"

#define INITIAL_ARRAY_SIZE 3
#define GROW_ARRAY_FACTOR 2

typedef int (*OC_comp_func)(const void*, const void*);

/* A complete type declaration for Ordered_container implemented as an array.*/
struct Ordered_container {
  OC_comp_fp_t comp_func;	/* pointer to comparison function  */
  void** array;			    /* pointer to array of pointers to void */
  int allocation;			/* current size of array */
  int size;				    /* number of items currently in the array */
};

/* Helper function for searching for an item. If an item exists with a data
 * pointer that compares equal to 'data_ptr', we return 1 and provide the caller
 * with the index of the item via the 'item_index' parameter. If no such item
 * exists, we return 0 and set 'item_index' to the position where the
 * searched-for item should be inserted. */
static int find_item_or_index(const struct Ordered_container* c_ptr,
                              const void* data_ptr,
                              OC_comp_func,
                              int* item_index);
static void initialize_array_helper(struct Ordered_container* c_ptr);
static void grow_array_helper(struct Ordered_container* c_ptr);

struct Ordered_container* OC_create_container(OC_comp_fp_t comp_func)
{
  struct Ordered_container* c_ptr =
      safe_malloc(sizeof(struct Ordered_container));

  /* Initialize container variables. */
  c_ptr->comp_func = comp_func;
  initialize_array_helper(c_ptr);

  g_Container_count++;

  return c_ptr;
}

void OC_destroy_container(struct Ordered_container* c_ptr)
{
  /* Free up the array. */
  free(c_ptr->array);

  g_Container_items_in_use -= c_ptr->size;
  g_Container_items_allocated -= c_ptr->allocation;

  /* After freeing all array items, free the container. */
  free(c_ptr);

  g_Container_count--;
}

void OC_clear(struct Ordered_container* c_ptr)
{
  free(c_ptr->array);

  g_Container_items_in_use -= c_ptr->size;
  g_Container_items_allocated -= c_ptr->allocation;

  /* Restore small, initial allocation. */
  initialize_array_helper(c_ptr); 
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
  assert(item_ptr);
  return *((void **)item_ptr);
}

void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr)
{
  void** array_end = c_ptr->array + c_ptr->size;
  void** data_ptr_ptr = (void **)item_ptr;

  /* Now, move all elements above the deleted item down. */
  while (data_ptr_ptr < array_end) {
    *data_ptr_ptr = *(data_ptr_ptr + 1);
    ++data_ptr_ptr;
  }

  c_ptr->size--;
  g_Container_items_in_use--;
}

void OC_insert(struct Ordered_container* c_ptr, void* data_ptr)
{
  int insertion_index;
  int i;
 
  /* If our allocation is full, grow the array. */
  if (c_ptr->size == c_ptr->allocation) {
    grow_array_helper(c_ptr);
  }

  /* Find the index where the item should be inserted. Ignore the return
   * value because we don't care if an item with an equal data_ptr already
   * exists. */
  find_item_or_index(c_ptr, data_ptr, c_ptr->comp_func, &insertion_index);

  /* Move all items above the insertion point "up" to make room. */
  for (i = c_ptr->size; i > insertion_index; --i) {
    c_ptr->array[i] = c_ptr->array[i - 1];
  }

  /* Actually insert the item. */
  c_ptr->array[insertion_index] = data_ptr;

  c_ptr->size++;
  g_Container_items_in_use++;
}

void* OC_find_item(const struct Ordered_container* c_ptr, const void* data_ptr)
{
  int index;

  int found = find_item_or_index(c_ptr, data_ptr, c_ptr->comp_func, &index);

  return (found) ? c_ptr->array + index : NULL;
}

void* OC_find_item_arg(const struct Ordered_container* c_ptr,
                       const void* arg_ptr,
                       OC_find_item_arg_fp_t fafp)
{
  int index;

  int found = find_item_or_index(c_ptr, arg_ptr, fafp, &index);

  return (found) ? c_ptr->array + index : NULL;
}

void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp)
{
  int i;

  for (i = 0; i < c_ptr->size; ++i) {
    afp(c_ptr->array[i]);
  }
}

int OC_apply_if(const struct Ordered_container* c_ptr, OC_apply_if_fp_t afp)
{
  int i;
  int return_val;

  for (i = 0, return_val = 0;
       i < c_ptr->size && (return_val = afp(c_ptr->array[i])) == 0;
       ++i);

  return return_val;
}

void OC_apply_arg(const struct Ordered_container* c_ptr,
                  OC_apply_arg_fp_t afp,
                  void* arg_ptr)
{
  int i;

  for (i = 0; i < c_ptr->size; ++i) {
    afp(c_ptr->array[i], arg_ptr);
  }
}

int OC_apply_if_arg(const struct Ordered_container* c_ptr,
                    OC_apply_if_arg_fp_t afp,
                    void* arg_ptr)
{
  int i;
  int return_val;

  for (i = 0, return_val = 0;
       i < c_ptr->size && (return_val = afp(c_ptr->array[i], arg_ptr)) == 0;
       ++i);

  return return_val;
}

static void grow_array_helper(struct Ordered_container* c_ptr) {
  int new_allocation;
  void** new_array;
  int i;

  assert(c_ptr->allocation == c_ptr->size);

  new_allocation = (c_ptr->allocation + 1) * GROW_ARRAY_FACTOR;
  new_array = safe_malloc(new_allocation * sizeof(void *));

  for (i = 0; i < c_ptr->size; ++i) {
    new_array[i] = c_ptr->array[i];
  }

  /* Free the old array. */
  free(c_ptr->array);

  /* Update the container to point to the new array. */
  c_ptr->array = new_array;
  c_ptr->allocation = new_allocation;

  /* Update the global. */
  g_Container_items_allocated += new_allocation - c_ptr->size;
}

static void initialize_array_helper(struct Ordered_container* c_ptr)
{
  c_ptr->array = safe_malloc(INITIAL_ARRAY_SIZE * sizeof(void *));
  c_ptr->allocation = INITIAL_ARRAY_SIZE;
  c_ptr->size = 0;

  g_Container_items_allocated += INITIAL_ARRAY_SIZE;
}

static int find_item_or_index(const struct Ordered_container* c_ptr,
                              const void* data_ptr,
                              OC_comp_func comp_func,
                              int* item_index)
{
  int comp_result;
  int low = 0;
  int high = c_ptr->size - 1;
  int mid;

  while (low <= high) {
    mid = (low + high) / 2;
    if ((comp_result = comp_func(data_ptr, c_ptr->array[mid])) < 0) {
      high = mid - 1;
    } else if (comp_result > 0) {
      low = mid + 1;
    } else {
      /* We've found the item. */
      *item_index = mid;
      return 1;
    }
  }

  /* We failed to find the item. */
  *item_index = high + 1;
  return 0;
}

