#include "Meeting.h"
#include "Person.h"
#include "Room.h"
#include "Utility.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

Commitment::operator< (const Commitment& c)
{
  if (!(c.room < room) && !(room < c.room)) {
    return meeting < c.meeting;
  }
  return room < c.room;
}

Person::Person(std::ifstream& is)
{
  if (!(is >> firstname) || !(is >> lastname) || !(is >> phoneno)) {
    throw_invalid_data();
  }
}

void Person::add_commitment(const Room& room, const Meeting& meeting)
{
  Commitment new_commitment(room, meeting);
  if (commitments.find(new_commitment) != commitment.end()) {
    throw Error{"Person is already committed at that time!"};
  }
  commitments.insert(new_commitment);
}

bool Person::has_commitments()
{
  return !commitments.empty();
}

void Person::print_commitments() {
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

void Person::remove_commitments_for_room(const Room& room)
{
  

void Person::save(std::ostream& os) const
{
  os << firstname << " " << lastname << " " << phoneno << std::endl;
}

std::ostream& operator<< (std::ostream& os, const Person& person)
{
  os << person.firstname << " " << person.lastname << " " << person.phoneno;
  return os;
}
