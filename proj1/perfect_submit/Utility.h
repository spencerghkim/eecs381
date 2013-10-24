#ifndef UTILITY_H_
#define UTILITY_H_

/* Include for declaration of FILE. */
#include <stdio.h>

/* TODO(wjbillin): Figure out how to use define in define. */
#define MAX_STR_LENGTH 63
#define STR_FORMAT "%63s"

struct Person;

char* alloc_char(int bytes);

int comp_func_person(const void* person_ptr1, const void* person_ptr2);
int comp_func_person_arg(const void* lastname, const void* person_ptr);

void free_char(char* char_ptr);

void print_person_wrapper(void* person_ptr);

/* Read in a string to the given pointer. Returns the result of scanf - the
 * number of successfully read arguments (or EOF).
 * NOTE: Assume that space has already been allocated for destination. */
int read_string(FILE* infile, char* destination);

/* Safely allocate memory. */
void* safe_malloc(int bytes);

#endif
