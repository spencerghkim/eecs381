#include "String.h"

#include <cstring>
#include <iostream>
#include <new>

using std::cout;
using std::endl;

char String::empty_string = '\0';
int String::number = 0;
int String::total_allocation = 0;
bool String::messages_wanted = false;

void String::swap(String& other) noexcept
{
  int size_temp = other.str_size;
  other.str_size = this->str_size;
  this->str_size = size_temp;
  int allocation_temp = other.allocation;
  other.allocation = this->allocation;
  this->allocation = allocation_temp;
  char* c_string_temp = other.c_string;
  other.c_string = this->c_string;
  this->c_string = c_string_temp;
}

String::String(const char* in_cstr) : c_string{&empty_string}, str_size{0}, allocation{0}
{
  if (messages_wanted) {
    cout << "Ctor: \"" << in_cstr  << "\"" << endl;
  }

  int in_size = strlen(in_cstr);
  if (in_size) {
    c_string = new char[in_size + 1];
    strcpy(c_string, in_cstr);
    str_size = in_size;
    allocation = in_size + 1;

    total_allocation += allocation;
  }

  ++number;
}

String::String(const String& original) :
    c_string{original.c_string}, str_size{original.str_size}, allocation{original.allocation}
{
  if (messages_wanted) {
    cout << "Copy ctor: \"" << original.c_string  << "\"" << endl;
  }

  if (original.str_size) {
    c_string = new char[original.str_size + 1];
    strcpy(c_string, original.c_string);
    allocation = original.str_size + 1;

    total_allocation += allocation;
  }

  ++number;
}

String::String(String&& original) noexcept :
    c_string{original.c_string}, str_size{original.str_size}, allocation{original.allocation}
{
  if (messages_wanted) {
    cout << "Move ctor: \"" << original.c_string  << "\"" << endl;
  }

  // TODO: can i functionalize this?
  original.c_string = &empty_string;
  original.str_size = 0;
  original.allocation = 0;

  --number;
}

String::~String() noexcept
{
  if (messages_wanted) {
    cout << "Dtor: \"" << c_string  << "\"" << endl;
  }

  if (str_size) {
    total_allocation -= allocation;
    delete[] c_string;
  }

  --number;
}

String& String::operator= (const String& rhs)
{
  if (messages_wanted) {
    cout << "Copy assign from String:  \"" << rhs.c_string << "\"" << endl;
  }

  String temp {rhs};
  swap(temp);
  return *this;
}

String& String::operator= (const char* rhs)
{
  if (messages_wanted) {
    cout << "Assign from C-string:  \"" << rhs << "\"" << endl;
  }

  String temp {rhs};
  swap(temp);
  return *this;
}

String& String::operator= (String&& rhs) noexcept
{
  if (messages_wanted) {
    cout << "Move assign from String:  \"" << rhs.c_string << "\"" << endl;
  }

  swap(rhs);
  return *this;
}

char& String::operator[] (int i)
{
  if (i < 0 || i >= str_size) {
    throw String_exception("Subscript out of range!");
  }
  return *(c_string + i);
}

const char& String::operator[] (int i) const
{
  return this->operator[](i);
}

void String::clear()
{
  String temp {};
  swap(temp);
}

String& String::operator += (char rhs)
{
  char tmp[2];
  tmp[0] = rhs;
  tmp[1] = '\0';
  String tmpStr {tmp};
  return (*this) += tmpStr;
}

String& String::operator += (const char* rhs)
{
  int append_size = strlen(rhs);
  int allocation_needed = str_size + append_size + 1;
  if (allocation < allocation_needed) {
    int new_allocation = 2 * allocation_needed;
    char* new_c_string = new char[new_allocation];
    
    total_allocation += (new_allocation - allocation);
    allocation = new_allocation;

    if (!str_size) {
      // String currently points to our static null character.
      strcpy(new_c_string, rhs);
      c_string = new_c_string;
    } else {
      // Copy the old contents of c_string into our new c_string.
      strcpy(new_c_string, c_string);
      strcat(new_c_string, rhs);
      delete[] c_string;
      c_string = new_c_string;
    }
  } else {
    strcat(c_string, rhs);
  }

  str_size += append_size;

  return *this;
}

String& String::operator += (const String& rhs)
{
  return this->operator += (rhs.c_string);
}

bool operator== (const String& lhs, const String& rhs)
{
  return (strcmp(lhs.c_str(), rhs.c_str()) == 0);
}

bool operator!= (const String& lhs, const String& rhs)
{
  return !(lhs == rhs);
}

bool operator> (const String& lhs, const String& rhs)
{
  return (strcmp(lhs.c_str(), rhs.c_str()) > 0);
}

bool operator< (const String& lhs, const String& rhs)
{
  return (strcmp(lhs.c_str(), rhs.c_str()) < 0);
}

String operator+ (const String& lhs, const String& rhs)
{
  String temp {lhs};
  temp += rhs;
  return temp;
}

std::ostream& operator<< (std::ostream& os, const String& str)
{
  return os << str.c_str();
}

std::istream& operator>> (std::istream& is, String& str)
{
  str.clear();
  char c;

  // Pull off any leading whitespace.
  while (is.good() && (c = is.peek()) && isspace(c)) {
    is.get();
  }

  while (is.good() && (c = is.peek()) && !isspace(c)) {
    str += is.get();
  }

  return is;
}

