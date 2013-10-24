#include <fstream>
#include <iostream>
#include "Meeting.h"
#include "Ordered_list.h"
#include "Person.h"
#include "String.h"
#include "Utility.h"

int normalized_time(int time);

Meeting::Meeting(std::ifstream& is,
                 const Ordered_list<const Person*, Less_than_ptr<const Person*> >& people) {

  int num_participants = 0;
  if (!(is >> time) || !(is >> topic) || !(is >> num_participants)) {
    throw_invalid_data();
  }

  auto person_it = participants.begin();
  for (; num_participants > 0; --num_participants) {
    String lastname;
    if (!(is >> lastname)) {
      throw_invalid_data();
    }
    Person probe{lastname};
    if ((person_it = people.find(&probe)) == people.end()) {
      throw_invalid_data();
    }
    participants.insert(*person_it);
  }
}

void Meeting::add_participant(const Person* p) {
  if (participants.find(p) != participants.end()) {
    throw Error{"This person is already a participant!"};
  }
  participants.insert(p);
}

bool Meeting::is_participant_present(const Person* p) const {
  return (participants.find(p) != participants.end());
}

void Meeting::remove_participant(const Person* p) {
  auto person_it = participants.begin();
  if ((person_it = participants.find(p)) == participants.end()) {
    throw Error{"This person is not a participant in the meeting!"};
  }
  participants.erase(person_it);
}

void Meeting::save(std::ostream& os) const {
  os << time << " " << topic << " " << participants.size() << std::endl;
  for (auto person_it = participants.begin(); person_it != participants.end(); ++person_it) {
    os << (*person_it)->get_lastname() << std::endl;
  }
}

bool Meeting::operator< (const Meeting& other) const {
  return (normalized_time(time) < normalized_time(other.time));
}

std::ostream& operator<< (std::ostream& os, const Meeting& meeting) {
  os << "Meeting time: " << meeting.time << ", Topic: " << meeting.topic << "\nParticipants:";
  if (meeting.participants.empty()) {
    os << " None" << std::endl;
  } else {
    os << std::endl;
    for (auto it = meeting.participants.begin(); it != meeting.participants.end(); ++it) {
      os << **it << std::endl;
    }
  }
  return os;
}

int normalized_time(int time) {
  return (time < 6) ? time + 12 : time;
}
