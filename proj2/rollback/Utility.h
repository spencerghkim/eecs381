#ifndef UTILITY_H_
#define UTILITY_H_

/* Utility functions, constants, and classes used by more than one other modules */

// a simple class for error exceptions - msg points to a C-string error message
struct Error {
	Error(const char* msg_ = "") :
		msg(msg_)
		{}
	const char* msg;
};

// Throws an invalid data Error exception.
void throw_invalid_data();

#endif
