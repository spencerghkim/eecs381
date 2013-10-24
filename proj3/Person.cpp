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
  bool operator() (const Commitment_t* c_1, const Commitment_t* c_2) {
    if (!(c_1->room < c_2->room) && !(c_2->room < c_1->room)) {
      return c_1->meeting->get_time() < c_2->meeting->get_time();
    }
    return c_1->room < c_2->room;
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
  Commitment_t c {room, meeting};
  
  Commitment_c::iterator it =
      std::lower_bound(commitments.begin(), commitments.end(), c, Commitment_comp());
  if ((it != commitments.end()) && (it->meeting == meeting)) {
    throw Error{"Person is already committed at that time!"};
  }
  commitments.insert(it, c);
}

struct Commitment_finder {
  void isAtTime
};

bool Person::has_commitment(int time)
{
  return commitments.find(time) != commitments.end();
}

bool Person::has_commitments() const
{
  return !commitments.empty();
}

using commitment_t = Commitment_c::value_type;
void Person::print_commitments() const
{
  // Print the commitments out in the specified order. We trade in poor performance here
  // (by re-ordering all of the commitments) for good performance when searching commitments
  // for a specific time (which is quick because commitments is keyed on time).
  std::set<commitment_t> printable_commitments;
  for_each(commitments.begin(),
           commitments.end(),
           [&printable_commitments] (commitment_t commitment) {
             printable_commitments.insert(commitment);
           });
  
  if (commitments.empty()) {
    std::cout << "No commitments" << std::endl;
    return;
  }
  
  for_each(printable_commitments.begin(),
           printable_commitments.end(),
           [] (commitment_t pair) {
    std::cout << "Room:" << pair.second.get_room_number();
    std::cout << " Time: " << pair.first;
    std::cout << " Topic: " << pair.second.get_Meeting(pair.first)->get_topic() << std::endl;
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
