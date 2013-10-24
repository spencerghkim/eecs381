#include "Meeting.h"
#include "Room.h"
#include "Utility.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>

Room::Room(std::ifstream& is,
           const Ordered_list<const Person*, Less_than_ptr<const Person*> >& people_list) {
  int num_meetings = 0;

  if (!(is >> room_number) || !(is >> num_meetings)) {
    throw_invalid_data();
  }

  for (; num_meetings > 0; --num_meetings) {
    Meeting meeting(is, people_list);
    meetings.push_back(meeting);
  }
}

void Room::add_Meeting(const Meeting& m) {
  Meetings_c::iterator low = lower_bound(meetings.begin(), meetings.end(), m);
  if (low != meetings.end() && low->get_time() == m.get_time()) {
    throw Error{"There is already a meeting at that time!"};
  }
  meetings.insert(low, m);
}

bool Room::is_Meeting_present(int time) const {
  Meeting probe{time};
  return binary_search(meetings.begin(), meetings.end(), probe);
}

Meetings_c::iterator get_Meeting_helper(Meetings_c meetings_c, int time)
{
  Meetings_c::iterator low = lower_bound(meetings_c.begin(), meetings_c.end(), m);
  if (low->get_time != time) {
    throw Error{"No meeting at that time!"};
  }
  return low;
}

Meeting& Room::get_Meeting(int time) const {
  return get_Meeting_helper(meetings, time);
}

void Room::remove_Meeting(int time) {
  auto meeting_it = get_Meeting_helper(meetings, time);
  meetings.erase(meeting_it);
}

/*bool Room::is_participant_present(const Person* person_ptr) const {
  for (auto meeting_it = meetings.begin(); meeting_it != meetings.end(); ++meeting_it) {
    if (meeting_it->is_participant_present(person_ptr)) {
      return true;
    }
  }
  return false;
}*/ 

void Room::save(std::ostream& os) const {
  os << room_number << " " << meetings.size() << std::endl;
  // Find out how to use bind/mem_fn.
  for_each(meetings.begin(), meetings.end(), [&os] (Meeting& m) { m.save(os); });
  /*for (auto meeting_it = meetings.begin(); meeting_it != meetings.end(); ++meeting_it) {
    meeting_it->save(os);
  }*/
}

std::ostream& operator<< (std::ostream& os, const Room& room) {
  os << "--- Room " << room.room_number << " ---" << std::endl;
  if (room.meetings.empty()) {
    os << "No meetings are scheduled" << std::endl;
  } else {
    for_each(meetings.begin(), meetings.end(), [&os] (Meeting& m) { os << m; });
/*    for (auto it = room.meetings.begin(); it != room.meetings.end(); ++it) {
      os << *it;
    }*/
  }
  return os;
}

