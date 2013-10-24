#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p1_globals.h"
#include "Person.h"
#include "Utility.h"

char* alloc_char(int bytes) {
  char* char_mem = safe_malloc(bytes);
  g_string_memory += bytes;

  return char_mem;
}

void free_char(char* char_ptr)
{
  /* Add one to strlen for the null character at the end. */
  g_string_memory -= (strlen(char_ptr) + 1);
  free(char_ptr);
}

int comp_func_person(const void* person_ptr1, const void* person_ptr2)
{
  return strcmp(get_Person_lastname((struct Person*)person_ptr1),
                get_Person_lastname((struct Person*)person_ptr2));
}

int comp_func_person_arg(const void* lastname, const void* person_ptr)
{
  return strcmp((char *)lastname,
                get_Person_lastname((struct Person*)person_ptr));
}

void wrapper_person_print(void* person_ptr)
{
  print_Person((struct Person *)person_ptr);
}

int read_string(FILE* infile, char* destination)
{
  return fscanf(infile, STR_FORMAT, destination);
}

void* safe_malloc(int bytes)
{
  void* memory = malloc(bytes);

  if (!memory) {
    printf("Memory allocation error. Could not allocate %d bytes of memory",
        bytes);
    exit(0);
  }

  return memory;
}

