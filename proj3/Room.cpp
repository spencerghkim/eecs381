#include "Meeting.h"
#include "Room.h"
#include "Utility.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std::placeholders;

using Person_c = std::set<Person*, Person_comp>;
using Meetings_c = std::map<int, Meeting*>;

Room::Room(std::ifstream& is, const Person_c& people_list) {
  int num_meetings = 0;

  if (!(is >> room_number) || !(is >> num_meetings)) {
    throw_invalid_data();
  }

  for (; num_meetings > 0; --num_meetings) {
    Meeting* meeting = new Meeting(is, people_list, *this);
    meetings.insert(std::make_pair(meeting->get_time(), meeting));
  }
}

void Room::add_Meeting(Meeting* new_meeting) {
  if (meetings.find(new_meeting->get_time()) != meetings.end()) {
    throw Error{"There is already a meeting at that time!"};
  }
  meetings.insert(std::make_pair(new_meeting->get_time(), new_meeting));
}

bool Room::is_Meeting_present(int time) const {
  return meetings.find(time) != meetings.end();
}

Meeting* Room::get_Meeting(int time) const
{
  std::cout << "get meeting called with time " << time << std::endl;
  if (meetings.find(time) == meetings.end()) {
    throw Error{"No meeting at that time!"};
  }
  return meetings.find(time)->second;
}

void Room::remove_Meeting(int time) {
  auto meeting_it = meetings.find(time);
  if (meeting_it == meetings.end()) {
    throw Error{"No meeting at that time!"};
  }
  
  delete meeting_it->second;
  meetings.erase(meeting_it);
}

void Room::clear_Meetings() {
  std::for_each(meetings.begin(),
                meetings.end(),
                [] (Meetings_c::value_type pair) { delete pair.second; });
  meetings.clear();
}

void Room::save(std::ostream& os) const {
  os << room_number << " " << meetings.size() << std::endl;
  std::for_each(meetings.begin(),
                meetings.end(),
                std::bind(&Meeting::save, std::bind(&Meetings_c::value_type::second, _1), ref(os)));
}

std::ostream& operator<< (std::ostream& os, const Room& room) {
  os << "--- Room " << room.room_number << " ---" << std::endl;
  if (room.meetings.empty()) {
    os << "No meetings are scheduled" << std::endl;
  } else {
    std::for_each(room.meetings.begin(),
                  room.meetings.end(),
                  [&os] (Meetings_c::value_type pair) { os << *(pair.second); });
  }
  return os;
}

