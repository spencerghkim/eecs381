#ifndef UTILITY_H_
#define UTILITY_H_

/* Include for declaration of FILE. */
#include <stdio.h>

/* This includes the terminating NULL character. */
#define MAX_STR_LENGTH 64
#define STR_FORMAT "%63s"

struct Person;

/* Allocate a char* of length bytes. This will also increase the global
 * variable keeping track of C-string memory usage. */
char* alloc_char(int bytes);

/* Free the given char pointer. This will also decrease the global variable
 * keeping track of C-string memory usage. */
void free_char(char* char_ptr);

/* Person-related utility functions. */
int comp_func_person(const void* person_ptr1, const void* person_ptr2);
int comp_func_person_arg(const void* lastname, const void* person_ptr);
void wrapper_person_print(void* person_ptr);

/* Read in a string to the given pointer. Returns the result of scanf - the
 * number of successfully read arguments (or EOF).
 * NOTE: Assume that space has already been allocated for destination. */
int read_string(FILE* infile, char* destination);

/* Safely allocate memory. */
void* safe_malloc(int bytes);

#endif
