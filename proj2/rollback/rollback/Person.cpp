#include "Person.h"
#include "String.h"
#include "Utility.h"
#include <fstream>
#include <iostream>

Person::Person(std::ifstream& is) {
  if (!(is >> firstname) || !(is >> lastname) || !(is >> phoneno)) {
    throw_invalid_data();
  }
}

void Person::save(std::ostream& os) const {
  os << firstname << " " << lastname << " " << phoneno << std::endl;
}

std::ostream& operator<< (std::ostream& os, const Person& person) {
  os << person.firstname << " " << person.lastname << " " << person.phoneno;
  return os;
}
