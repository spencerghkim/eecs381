#include "Meeting.h"
#include "Person.h"
#include "Room.h"
#include "Utility.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

using Commitment_c = std::list<Commitment_t>;

struct Commitment_comp {
  bool operator() (const Commitment_t c_1, const Commitment_t c_2) {
    if (!(c_1.room < c_2.room) && !(c_2.room < c_1.room)) {
      return c_1.meeting->get_time() < c_2.meeting->get_time();
    }
    return c_1.room < c_2.room;
  }
};

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
      std::lower_bound(commitments.begin(), commitments.end(), c, Commitment_comp());
  commitments.insert(it, c);
}

bool Person::has_commitment(int time)
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

using commitment_t = Commitment_c::value_type;
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

void Person::remove_commitment(int time) noexcept
{
  // TODO(wjbillin): Figure out how to do this with for_each or bind or something.
  for (Commitment_c::iterator it = commitments.begin(); it != commitments.end(); ++it) {
    if (it->meeting->get_time() == time) {
      commitments.erase(it);
      break;
    }
  }
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
