#include "Meeting.h"
#include "Room.h"
#include "Utility.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>

using namespace std::placeholders;

using Person_c = std::set<Person*, Person_comp>;
using Meetings_c = std::list<Meeting>;

Room::Room(std::ifstream& is, const Person_c& people_list) {
  int num_meetings = 0;

  if (!(is >> room_number) || !(is >> num_meetings)) {
    throw_invalid_data();
  }

  for (; num_meetings > 0; --num_meetings) {
    Meeting meeting(is, people_list, *this);
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
  Meetings_c::iterator low = lower_bound(meetings_c.begin(), meetings_c.end(), Meeting{time});
  if (low->get_time() != time) {
    throw Error{"No meeting at that time!"};
  }
  return low;
}

Meeting& Room::get_Meeting(int time) const {
  return (*get_Meeting_helper(meetings, time));
}

void Room::remove_Meeting(int time) {
  auto meeting_it = get_Meeting_helper(meetings, time);
  meetings.erase(meeting_it);
}

void Room::save(std::ostream& os) const {
  os << room_number << " " << meetings.size() << std::endl;
  std::for_each(meetings.begin(), meetings.end(), std::bind(&Meeting::save, _1, ref(os)));
}

std::ostream& operator<< (std::ostream& os, const Room& room) {
  os << "--- Room " << room.room_number << " ---" << std::endl;
  if (room.meetings.empty()) {
    os << "No meetings are scheduled" << std::endl;
  } else {
    std::for_each(room.meetings.begin(), room.meetings.end(), [&os] (Meeting m) { os << m; });
  }
  return os;
}

