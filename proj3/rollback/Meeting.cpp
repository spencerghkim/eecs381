#include "Meeting.h"
#include "Person.h"
#include "Utility.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std::placeholders;
using std::string;
using std::map;
using std::vector;

using Person_c = vector<Person*>;

Person_c::iterator

int normalized_time(int time);

Meeting::Meeting(std::ifstream& is, const Person_c& persons_c)
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
    auto person_it = std::lower_bound(persons_c.begin(),
                                      persons_c.end(),
                                      &probe,
                                      Person_comp);
    if ((person_it == persons_c.end()) || person_it->lastname != lastname) {
      throw_invalid_data();
    }

    // Insert person into our participants. Since we saved them in order,
    // we can read them in order.
    participants.push_back(&probe);
  }
}

~Meeting::Meeting()
{
  // Let the Persons know that they don't have commitments at this time any longer.
  std::for_each(participants.begin(),
                participants.end(),
                std::bind(&Person::remove_commitment, _1, time));
}

void Meeting::add_participant(const Person* person_ptr)
{
  auto participant_it = std::lower_bound(participants.begin(),
                                         participants.end(),
                                         person_ptr,
                                         Person_comp);
  if ((participant_it != participants.end()) && (*participant_it) == person_ptr) {
    throw Error{"This person is already a participant!"};
  }
  participants.insert(participant_it, person_ptr);
}

bool Meeting::is_participant_present(const Person* person_ptr) const
{
  auto participant_it = std::lower_bound(participants.begin(),
                                         participants.end(),
                                         person_ptr,
                                         Person_comp);
  return ((participant_it != participants.end()) && (*participant_it) == person_ptr);
}

void Meeting::remove_participant(Person* person_ptr)
{
 auto participant_it = std::lower_bound(participants.begin(),
                                        participants.end(),
                                        person_ptr,
                                        Person_comp);
  if ((participant_it == participants.end()) || (*participant_it) != person_ptr) {
    throw Error{"This person is not a participant in the meeting!"};
  }
  participants.erase(participant_it);

  // Let the Person know that they don't have a commitment at this time any longer.
  person_ptr->remove_commitment(time);
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

int normalized_time(int time)
{
  return (time < 6) ? time + 12 : time;
}
