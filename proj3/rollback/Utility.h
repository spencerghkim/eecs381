#ifndef UTILITY_H_
#define UTILITY_H_

#include "Person.h"

// a simple class for error exceptions - msg points to a C-string error message
struct Error {
	Error(const char* msg_ = "") :
		msg(msg_)
		{}
	const char* msg;
};

// Comparison function for person pointers.
struct Person_ptr_comp {
  bool operator() (const Person* person_ptr_1, const Person* person_ptr_2) const {
    return person_ptr_1->get_lastname() < person_ptr_2->get_lastname();
  }
};

// Converts a 12-hour time into a 24-hour time for comparison purposes.
int normalized_time(const int time);

// Throws an invalid data Error exception.
void throw_invalid_data();

#endif
