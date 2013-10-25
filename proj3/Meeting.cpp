#include "Meeting.h"
#include "Person.h"
#include "Utility.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <string>

using namespace std::placeholders;
using std::string;

using Person_c = std::set<Person*, Person_ptr_comp>;

Meeting::Meeting(std::ifstream& is, const Person_c& persons_c, const Room* room)
{
  int num_participants = 0;
  if (!(is >> time) || !(is >> topic) || !(is >> num_participants)) {
    throw_invalid_data();
  }

  for (; num_participants > 0; --num_participants) {
    string lastname;
    if (!(is >> lastname)) {
      throw_invalid_data();
    }

    // Find person in the master list of people.
    Person probe{lastname};
    auto person_it = persons_c.begin();
    if ((person_it = persons_c.find(&probe)) == persons_c.end()) {
      throw_invalid_data();
    }

    (*person_it)->add_commitment(room, this);
    // Insert person into our participants. Since we saved them in order,
    // we can read them in order.
    participants.push_back(*person_it);
  }
}

Meeting::~Meeting() {
  std::for_each(participants.begin(),
                participants.end(),
                std::bind(&Person::remove_commitment, _1, time));
}

void Meeting::add_participant(Person* person_ptr, const Room* room)
{
  auto participant_it = std::lower_bound(participants.begin(),
                                         participants.end(),
                                         person_ptr,
                                         Person_ptr_comp());
  if ((participant_it != participants.end()) && (*participant_it) == person_ptr) {
    throw Error{"This person is already a participant!"};
  }

  // Tell the person they now have a commitment. Throw exception if they're
  // already committed at this time.
  person_ptr->add_commitment(room, this);
  participants.insert(participant_it, person_ptr);
}

bool Meeting::is_participant_present(const Person* person_ptr) const
{
  auto participant_it = std::lower_bound(participants.begin(),
                                         participants.end(),
                                         person_ptr,
                                         Person_ptr_comp());
  return ((participant_it != participants.end()) && (*participant_it) == person_ptr);
}

void Meeting::remove_participant(Person* person_ptr)
{
 auto participant_it = std::lower_bound(participants.begin(),
                                        participants.end(),
                                        person_ptr,
                                        Person_ptr_comp());
  if ((participant_it == participants.end()) || (*participant_it) != person_ptr) {
    throw Error{"This person is not a participant in the meeting!"};
  }
  participants.erase(participant_it);

  // Let the Person know that they don't have a commitment at this time any longer.
  person_ptr->remove_commitment(time);
}

void Meeting::transfer_participants(Meeting* new_meeting, const Room* new_room)
{
  // First, check if any of the participants has a conflict at the new time. Note
  // that we only have to check for conflicts if the times are different - if they
  // are the same, then we know everyone is available.
  int proposed_time = new_meeting->get_time();
  if (proposed_time != time) {
    std::for_each(participants.begin(),
                  participants.end(),
                  [proposed_time] (Person* person_ptr) {
        if (person_ptr->has_commitment(proposed_time)) {
          throw Error{"A participant is already committed at the new time!"};
        }
    });
  }
  
  // Next, tell all the people that their commitment has changed.
  std::for_each(participants.begin(), participants.end(), [&] (Person* person_ptr) {
      person_ptr->remove_commitment(time);
      person_ptr->add_commitment(new_room, new_meeting);
  });
  
  // Finally, transfer the participants list.
  new_meeting->participants = participants;
  participants.clear();
}

void Meeting::save(std::ostream& os) const
{
  os << time << " " << topic << " " << participants.size() << std::endl;
  std::for_each(participants.begin(), participants.end(), [&os] (Person* person_ptr) {
      os << person_ptr->get_lastname() << std::endl;
  });
}

bool Meeting::operator< (const Meeting& other) const
{
  return (normalized_time(time) < normalized_time(other.time));
}

std::ostream& operator<< (std::ostream& os, const Meeting& meeting)
{
  os << "Meeting time: " << meeting.time << ", Topic: " << meeting.topic << "\nParticipants:";
  if (meeting.participants.empty()) {
    os << " None" << std::endl;
  } else {
    os << std::endl;
    std::for_each(meeting.participants.begin(),
                  meeting.participants.end(),
                  [&os] (Person* person_ptr) {
        os << *person_ptr << std::endl;
    });
  }
  return os;
}
