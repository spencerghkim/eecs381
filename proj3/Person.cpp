#include "Meeting.h"
#include "Person.h"
#include "Room.h"
#include "Utility.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

bool Commitment_comp::operator()(const commitment_t &c_1, const commitment_t &c_2)
{
  if (!(c_1.second < c_2.second) && !(c_2.second < c_1.second)) {
    return c_1.first < c_2.first;
  }
  return c_1.second < c_2.second;
}

Person::Person(std::ifstream& is)
{
  if (!(is >> firstname) || !(is >> lastname) || !(is >> phoneno)) {
    throw_invalid_data();
  }
}

void Person::add_commitment(int time, const Room& room)
{
  if (commitments.find(time) != commitments.end()) {
    throw Error{"Person is already committed at that time!"};
  }
  commitments.insert(std::make_pair(time, room));
}

bool Person::has_commitment(int time)
{
  return commitments.find(time) != commitments.end();
}

bool Person::has_commitments() const
{
  return !commitments.empty();
}

void Person::print_commitments() const
{
  for_each(commitments.begin(), commitments.end(), [] (Commitment_c::value_type pair) {
    std::cout << "Room:" << pair.second.get_room_number();
    std::cout << " Time: " << pair.first;
    std::cout << " Topic: " << pair.second.get_Meeting(pair.first).get_topic() << std::endl;
  });
}

void Person::remove_commitment(int time) noexcept
{
  commitments.erase(time);
} 

void Person::save(std::ostream& os) const
{
  os << firstname << " " << lastname << " " << phoneno << std::endl;
}

std::ostream& operator<< (std::ostream& os, const Person& person)
{
  os << person.firstname << " " << person.lastname << " " << person.phoneno;
  return os;
}
