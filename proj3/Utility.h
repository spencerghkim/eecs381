#ifndef UTILITY_H_
#define UTILITY_H_

#include "Person.h"

/* Utility functions, constants, and classes used by more than one other modules */

// a simple class for error exceptions - msg points to a C-string error message
struct Error {
	Error(const char* msg_ = "") :
		msg(msg_)
		{}
	const char* msg;
};

struct Person_comp {
  bool operator() (const Person* person_ptr_1, const Person* person_ptr_2) const {
    return person_ptr_1->get_lastname() < person_ptr_2->get_lastname();
  }
};

// Throws an invalid data Error exception.
void throw_invalid_data();

#endif
