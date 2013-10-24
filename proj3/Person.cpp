#include "Meeting.h"
#include "Person.h"
#include "Room.h"
#include "Utility.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

Person::Person(std::ifstream& is)
{
  if (!(is >> firstname) || !(is >> lastname) || !(is >> phoneno)) {
    throw_invalid_data();
  }
}

void Person::add_commitment(int time, const Room& room)
{
  if (commitments.find(time) != commitment.end()) {
    throw Error{"Person is already committed at that time!"};
  }
  commitments[time] = room;
}

bool Person::has_commitment(int time)
{
  return (commitments.find(time) != commitments.end());

bool Person::has_commitments()
{
  return !commitments.empty();
}

void Person::print_commitments() {
  // We want to print the commitments in order by Room, then time. Dump the
  // time/rooms into a set to sort them accordingly.

  for_each(commitments.begin(), commitments.end(), [] (Commitments_c::value_type pair) {
      cout << "Room:" << pair.second.get_room_number();
      cout << " Time: " << pair.first;
      cout << " Topic: " << pair.second.get_Meeting(pair.first).get_topic() << std::endl;
  });
}

void Person::remove_commitment(int time)
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
