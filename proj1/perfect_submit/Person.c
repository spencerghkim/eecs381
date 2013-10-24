#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Person.h"
#include "p1_globals.h"
#include "Utility.h"

#define OUTPUT_FORMAT_STRING "%s %s %s\n"

/* a Person consist of pointers to C-strings for names and phone number */
struct Person {
	char* firstname;
	char* lastname;
	char* phoneno;
};

void print_helper(const struct Person* person_ptr, FILE* out);

struct Person* create_Person(
    const char* firstname, const char* lastname, const char* phoneno)
{
  struct Person* new_person =
      (struct Person*)safe_malloc(sizeof(struct Person));
  
  /* Dynamically allocate memory for the string data. */
  new_person->firstname = alloc_char(strlen(firstname) + 1);
  new_person->lastname = alloc_char(strlen(lastname) + 1);
  new_person->phoneno = alloc_char(strlen(phoneno) + 1);

  /* Actually copy the string data into the new person. */
  strcpy(new_person->firstname, firstname);
  strcpy(new_person->lastname, lastname);
  strcpy(new_person->phoneno, phoneno);

  return new_person;
}

void destroy_Person(struct Person* person_ptr)
{
  free_char(person_ptr->firstname);
  free_char(person_ptr->lastname);
  free_char(person_ptr->phoneno);

  free(person_ptr);
}

const char* get_Person_lastname(const struct Person* person_ptr)
{
  return person_ptr->lastname;
}

void print_Person(const struct Person* person_ptr)
{
  print_helper(person_ptr, stdout);
}

void save_Person(const struct Person* person_ptr, FILE* outfile)
{
  print_helper(person_ptr, outfile);
}

struct Person* load_Person(FILE* infile) {
  char lastname[MAX_STR_LENGTH + 1];
  char firstname[MAX_STR_LENGTH + 1];
  char phoneno[MAX_STR_LENGTH + 1];

  if (read_string(infile, firstname) != 1||
      read_string(infile, lastname) != 1||
      read_string(infile, phoneno) != 1) {
    /* There was a read error somewhere along the line. */
    return NULL;
  }

  return create_Person(firstname, lastname, phoneno);
} 

void print_helper(const struct Person* person_ptr, FILE* out)
{
  fprintf(out, OUTPUT_FORMAT_STRING, person_ptr->firstname,
                                     person_ptr->lastname,
                                     person_ptr->phoneno);
}
                          
