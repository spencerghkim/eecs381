#include "Meeting.h"
#include "Person.h"
#include "Room.h"
#include "Utility.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

using namespace std::placeholders;

bool Person::Commitment_t::operator< (const Commitment_t other) const
{
  if (!(this->room < other.room) && !(other.room < this->room)) {
    return this->meeting->get_time() < other.meeting->get_time();
  }
  return this->room < other.room;
}

Person::Person(std::ifstream& is)
{
  if (!(is >> firstname) || !(is >> lastname) || !(is >> phoneno)) {
    throw_invalid_data();
  }
}

void Person::add_commitment(const Room* room, const Meeting* meeting)
{
  if (has_commitment(meeting->get_time())) {
    throw Error{"Person is already committed at that time!"};
  }
  
  Commitment_t c {room, meeting};
  Commitment_c::iterator it =
      std::lower_bound(commitments.begin(), commitments.end(), c);
  commitments.insert(it, c);
}

void Person::remove_commitment(int time) noexcept
{
  commitments.remove_if([time] (Commitment_t commitment) {
    if (commitment.meeting->get_time() == time) {
      return true;
    }
    return false;
  });
}

bool Person::has_commitment(int time) const
{
  for (Commitment_t c : commitments) {
    if (c.meeting->get_time() == time) {
      return true;
    }
  }
  return false;
}

bool Person::has_commitments() const
{
  return !commitments.empty();
}

void Person::print_commitments() const
{
  if (commitments.empty()) {
    std::cout << "No commitments" << std::endl;
    return;
  }
  
  for_each(commitments.begin(),
           commitments.end(),
           [] (Commitment_t commitment) {
    std::cout << "Room:" << commitment.room->get_room_number();
    std::cout << " Time: " << commitment.meeting->get_time();
    std::cout << " Topic: " << commitment.meeting->get_topic() << std::endl;
  });
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

std::ostream& operator<< (std::ostream& os, const Person* person)
{
  os << *person;
  return os;
}
