#ifndef UTILITY_H
#define UTILITY_H

#include <string>

#define UNIQUE_STRING_PREFIX_SIZE 2

/* Utility functions and classes used by other modules */

// a simple class for error exceptions - msg points to a std::string error message
struct Error {
	Error(const std::string& in_msg = "") :
  msg(in_msg)
  {}
	const std::string msg;
};

#endif

