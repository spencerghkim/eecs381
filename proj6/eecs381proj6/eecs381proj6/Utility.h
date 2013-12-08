#ifndef UTILITY_H_
#define UTILITY_H_

#include <string>

/* Utility functions and classes used by other modules */

// a simple class for error exceptions - msg points to a std::string error message
struct Error {
	Error(const std::string& in_msg = "") :
  msg(in_msg)
  {}
	const std::string msg;
};

#endif

