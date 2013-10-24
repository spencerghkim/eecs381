#include "Meeting.h"
#include "Ordered_list.h"
#include "Room.h"
#include "Utility.h"

#include <fstream>
#include <iostream>

Ordered_list<Meeting>::Iterator get_Meeting_helper(
    int time, const Ordered_list<Meeting>& meetings);

Room::Room(std::ifstream& is,
           const Ordered_list<const Person*, Less_than_ptr<const Person*> >& people_list) {
  int num_meetings = 0;

  if (!(is >> room_number) || !(is >> num_meetings)) {
    throw_invalid_data();
  }

  for (; num_meetings > 0; --num_meetings) {
    Meeting meeting(is, people_list);
    meetings.insert(meeting);
  }
}

void Room::add_Meeting(const Meeting& m) {
  if (meetings.find(m) != meetings.end()) {
    throw Error{"There is already a meeting at that time!"};
  }
  meetings.insert(m);
}

bool Room::is_Meeting_present(int time) const {
  Meeting probe{time};
  return (meetings.find(probe) != meetings.end());
}

Meeting& Room::get_Meeting(int time) const {
 return *(get_Meeting_helper(time, meetings));
}

void Room::remove_Meeting(int time) {
  auto meeting_it = get_Meeting_helper(time, meetings);
  meetings.erase(meeting_it);
}

bool Room::is_participant_present(const Person* person_ptr) const {
  for (auto meeting_it = meetings.begin(); meeting_it != meetings.end(); ++meeting_it) {
    if (meeting_it->is_participant_present(person_ptr)) {
      return true;
    }
  }
  return false;
} 

void Room::save(std::ostream& os) const {
  os << room_number << " " << meetings.size() << std::endl;
  for (auto meeting_it = meetings.begin(); meeting_it != meetings.end(); ++meeting_it) {
    meeting_it->save(os);
  }
}

std::ostream& operator<< (std::ostream& os, const Room& room) {
  os << "--- Room " << room.room_number << " ---" << std::endl;
  if (room.meetings.empty()) {
    os << "No meetings are scheduled" << std::endl;
  } else {
    for (auto it = room.meetings.begin(); it != room.meetings.end(); ++it) {
      os << *it;
    }
  }
  return os;
}

Ordered_list<Meeting>::Iterator get_Meeting_helper(
    int time, const Ordered_list<Meeting>& meetings) {
  auto meeting_it = meetings.begin();
  Meeting probe{time};
  if ((meeting_it = meetings.find(probe)) == meetings.end()) {
    throw Error{"No meeting at that time!"};
  }
  return meeting_it;
}

