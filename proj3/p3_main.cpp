#include "Meeting.h"
#include "Room.h"
#include "Person.h"
#include "Utility.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <new>
#include <set>
#include <string>
#include <vector>

using namespace std;
using namespace std::placeholders;

using Room_c = map<int, Room*>;
using Person_c = set<Person *, Person_comp>;

using Room_it = Room_c::iterator;
using Person_it = Person_c::iterator;

struct Containers {
  Room_c rooms;
  Person_c persons;
};

using top_level_func_ptr = void (*)(Containers&);
using Function_map = map<string, top_level_func_ptr>;

void add_meeting(Containers& containers);
void add_participant(Containers& containers);
void add_person(Containers& containers);
void add_room(Containers& containers);

void delete_person(Containers& containers);
void delete_room(Containers& containers);
void delete_room_all(Containers& containers);
void delete_meeting(Containers& containers);
void delete_participant(Containers& containers);
void delete_meeting_all(Containers& containers);
void delete_person_all(Containers& containers);
void delete_all(Containers& containers);

void load_data(Containers& containers);
Person_c load_persons(ifstream& is);
Room_c load_rooms(ifstream& is, const Person_c& persons_c);

void print_commitments(Containers& containers);
void print_meeting(Containers& containers);
void print_meeting_all(Containers& containers);
void print_memory(Containers& containers);
void print_person(Containers& containers);
void print_person_all(Containers& containers);
void print_room(Containers& containers);

void reschedule_meeting(Containers& containers);

void save_data(Containers& containers);
void save_persons(ofstream& ofs, const Person_c& persons_c);
void save_rooms(ofstream& ofs, const Room_c& rooms_c);

// Utility functions in p2_main.cpp

void print_bad_command_and_clear();

Meeting* read_and_find_meeting(Room_c& rooms_c);
Person* read_and_find_person(const Person_c& persons_c);
Room* read_and_find_room(Room_c& rooms_c);

int read_int();
int read_int_from_file(ifstream& ifs);
int read_room_number();
int read_time();

void populate_top_level_func_map(Function_map& func_map);

int main() {
  Containers containers {};

  Function_map func_map {};
  populate_top_level_func_map(func_map);

  char first_letter;
  char second_letter;

  while (1) {
    cout << "\nEnter command: ";
    cin >> first_letter >> second_letter;

    try {

      string command = string{&first_letter, 1} + string{&second_letter, 1};
      
      // Explicitly check for the quit command.
      if (command == "qq") {
        delete_all(containers);
        return 0;
      }

      Function_map::iterator command_it = func_map.find(command);
      if (command_it == func_map.end()) {
        print_bad_command_and_clear();
      } else {
        (command_it->second)(containers);
      }

    } catch (Error& e) {
      cin.clear();
      while (cin.get() != '\n');
      cout << e.msg << endl;
    } catch (std::bad_alloc& bad_alloc) {
      cerr << "Memory allocation threw bad allocation!" << endl;
      delete_all(containers);
      return 0;
    } catch (...) {
      cerr << "Unknown exception!" << endl;
      delete_all(containers);
      return 0;
    }
  }

  return 0;
}

void populate_top_level_func_map(Function_map& func_map)
{
  func_map["ai"] = add_person;
  func_map["am"] = add_meeting;
  func_map["ap"] = add_participant;
  func_map["ar"] = add_room;
  func_map["da"] = delete_all;
  func_map["dg"] = delete_person_all;
  func_map["di"] = delete_person;
  func_map["dm"] = delete_meeting;
  func_map["dp"] = delete_participant;
  func_map["ds"] = delete_meeting_all;
  func_map["dr"] = delete_room;
  func_map["ld"] = load_data;
  func_map["pa"] = print_memory;
  func_map["pc"] = print_commitments;
  func_map["pg"] = print_person_all;
  func_map["pi"] = print_person;
  func_map["pm"] = print_meeting;
  func_map["pr"] = print_room;
  func_map["ps"] = print_meeting_all;
  func_map["rm"] = reschedule_meeting;
  func_map["sd"] = save_data;
}

void add_meeting(Containers& containers)
{
  Room* room = read_and_find_room(containers.rooms);
  int time = read_time();
  string topic;
  cin >> topic;
  Meeting* new_meeting = new Meeting(time, topic);
  room->add_Meeting(new_meeting);

  cout << "Meeting added at " << time << endl;
}

void add_participant(Containers& containers) 
{
  Room* room = read_and_find_room(containers.rooms);
  int time = read_time();
  Meeting* meeting_ptr = room->get_Meeting(time);
  Person* person_ptr = read_and_find_person(containers.persons);

  if (meeting_ptr->is_participant_present(person_ptr)) {
    throw Error{"This person is already a participant!"};
  }

  meeting_ptr->add_participant(person_ptr, room);

  cout << "Participant " << person_ptr->get_lastname() << " added" << endl;
}

void add_person(Containers& containers)
{
  string firstname, lastname, phoneno;
  cin >> firstname >> lastname >> phoneno;

  Person probe {lastname};
  if (containers.persons.find(&probe) != containers.persons.end()) {
    throw Error{"There is already a person with this last name!"};
  }
  Person* new_person = new Person(firstname, lastname, phoneno);
  containers.persons.insert(new_person);

  cout << "Person " << lastname << " added" << endl;
}

void add_room(Containers& containers)
{
  int room_number = read_room_number();
  if (containers.rooms.find(room_number) != containers.rooms.end()) {
    throw Error{"There is already a room with this number!"};
  }
  Room room {room_number};
  containers.rooms.insert(std::make_pair(room_number, room));

  cout << "Room " << room_number << " added" << endl;
}

void delete_person(Containers& containers)
{
  Person* person_ptr = read_and_find_person(containers.persons);
  if (person_ptr->has_commitments()) {
    throw Error{"This person is a participant in a meeting!"};
  }

  cout << "Person " << person_ptr->get_lastname() << " deleted" << endl;

  delete person_ptr;
  containers.persons.erase(person_ptr);
}

void delete_room(Containers& containers)
{
  Room& room = read_and_find_room(containers.rooms);
  room.clear_Meetings();

  cout << "Room " << room.get_room_number() << " deleted" << endl;
  containers.rooms.erase(room.get_room_number());
}

void delete_room_all(Containers& containers)
{
  for_each(containers.rooms.begin(),
           containers.rooms.end(),
           bind(&Room::clear_Meetings, bind(&Room_c::value_type::second, _1)));
  containers.rooms.clear();
  cout << "All rooms and meetings deleted" << endl;
}

void delete_meeting(Containers& containers)
{
  Room& room = read_and_find_room(containers.rooms);
  int time = read_time();
  room.remove_Meeting(time);
 
  cout << "Meeting at " << time << " deleted" << endl;
}

void delete_participant(Containers& containers)
{
  Meeting* meeting_ptr = read_and_find_meeting(containers.rooms);
  Person* person_ptr = read_and_find_person(containers.persons);
  meeting_ptr->remove_participant(person_ptr);

  cout << "Participant " << person_ptr->get_lastname() << " deleted" << endl;
}

void delete_meeting_all(Containers& containers)
{
  for_each(containers.rooms.begin(),
           containers.rooms.end(),
           bind(&Room::clear_Meetings, bind(&Room_c::value_type::second, _1)));
  cout << "All meetings deleted" << endl;
}

void delete_person_all(Containers& containers)
{
  for_each(containers.rooms.begin(), containers.rooms.end(), [] (Room_c::value_type& pair) {
    if (pair.second.has_Meetings()) {
      throw Error{"Cannot clear people list unless there are no meetings!"};
    }
  });

  // Before clearing the people, we need to go through and free the pointers.
  for (auto person_ptr : containers.persons) {
    delete person_ptr;
  }
  containers.persons.clear();

  cout << "All persons deleted" << endl;
}

void delete_all(Containers& containers)
{
  delete_room_all(containers);
  delete_person_all(containers);
}

void load_data(Containers& containers)
{
  string filename;
  cin >> filename;
  ifstream ifs(filename.c_str(), ifstream::in);
  
  if (!ifs.is_open()) {
    throw Error{"Could not open file!"};
  }

  // Store a backup copy of containers.
  Containers containers_backup = containers;
  
  // Clear the containers. If an exception is thrown during data load, we can safely delete
  // these containers (including dynamically allocated memory) without touching the structures
  // and dynamically allocated memory in the backup copy.
  containers.persons.clear();
  containers.rooms.clear();
  try {
    containers.persons = load_persons(ifs);
    containers.rooms = load_rooms(ifs, containers.persons);

  } catch (Error& e) {
    delete_all(containers);
    containers = containers_backup;

    ifs.close();
    throw e;
  }

  ifs.close();
  cout << "Data loaded" << endl;
}

Person_c load_persons(ifstream& ifs)
{
  Person_c persons_c {};
  for (int num_persons = read_int_from_file(ifs); num_persons > 0; --num_persons) {
    Person* new_person = new Person(ifs);
    persons_c.insert(new_person);
  }
  return persons_c;
}

Room_c load_rooms(ifstream& ifs, const Person_c& persons_c)
{
  Room_c rooms_c {};
  for (int num_rooms = read_int_from_file(ifs); num_rooms > 0; --num_rooms) {
    Room new_room(ifs, persons_c);
    
    // We should have saved rooms in order, so we can load them in order.
    rooms_c.insert(make_pair(new_room.get_room_number(), new_room));
  }
  return rooms_c;
}

void print_commitments(Containers& containers)
{
  Person* person_ptr = read_and_find_person(containers.persons);
  person_ptr->print_commitments();
}

void print_meeting(Containers& containers)
{
  Meeting* meeting = read_and_find_meeting(containers.rooms);
  cout << *meeting;
}
 
void print_meeting_all(Containers& containers)
{
  if (containers.rooms.empty()) {
    cout << "List of rooms is empty" << endl;
    return;
  }

  cout << "Information for " << containers.rooms.size() << " rooms:" << endl;
  for_each(containers.rooms.begin(), containers.rooms.end(), [] (Room_c::value_type& pair) {
    cout << pair.second;
  });
}

struct meetingSum {
  void add(int num)
    { total += num; }
  
  int get_total()
    { return total; }
  
  int total {0};
};

void print_memory(Containers& containers)
{
  cout << "Memory allocations:" << endl;
  cout << "Persons: " << containers.persons.size() << endl;

  // Sum up number of meetings.
  meetingSum sumer {};
  for_each(containers.rooms.begin(), containers.rooms.end(), [&sumer] (Room_c::value_type pair) {
    sumer.add(pair.second.get_number_Meetings());
  });
  cout << "Meetings: " << sumer.get_total() << endl;
  cout << "Rooms: " << containers.rooms.size() << endl;
}

void print_person(Containers& containers)
{
  Person* person_ptr = read_and_find_person(containers.persons);
  cout << *person_ptr << endl;
}

void print_person_all(Containers& containers)
{
  if (containers.persons.empty()) {
    cout << "List of people is empty" << endl;
  } else {
    cout << "Information for " << containers.persons.size() << " people:" << endl;
    for (auto person_ptr : containers.persons) {
      cout << *person_ptr << endl;
    }
  }
}

void print_room(Containers& containers)
{
  Room& room = read_and_find_room(containers.rooms);
  cout << room;
}

void reschedule_meeting(Containers& containers)
{
  Room& old_room = read_and_find_room(containers.rooms);
  int old_time = read_time();
  Meeting* old_Meeting = old_room.get_Meeting(old_time);

  Room& new_room = read_and_find_room(containers.rooms);
  int new_time = read_time();
  
  if (new_room.get_room_number() == old_room.get_room_number() &&
      old_Meeting->get_time() == new_time) {
    throw Error{"No change made to schedule"};
  }
  
  if (new_room.is_Meeting_present(new_time)) {
    throw Error{"There is already a meeting at that time!"};
  }

  Meeting* new_meeting = new Meeting{new_time, old_Meeting->get_topic()};
  
  // If transfer_participants fails, we are in charge of deleting the pointer, since
  // we never added the meeting to a room.
  try {
    old_Meeting->transfer_participants(new_meeting, new_room);
  } catch (Error e) {
    delete new_meeting;
  }

  // Actually insert the new meeting. Should not fail.
  new_room.add_Meeting(new_meeting);

  // If we get here, we're safe to remove the meeting from the old room.
  old_room.remove_Meeting(old_time);

  cout << "Meeting rescheduled to room " << new_room.get_room_number();
  cout << " at " << new_time << endl;
}

void save_data(Containers& containers)
{
  string filename;
  cin >> filename;
  ofstream ofs(filename.c_str(), ifstream::out);
  
  if (!ofs.is_open()) {
    throw Error{"Could not open file!"};
  }

  save_persons(ofs, containers.persons);
  save_rooms(ofs, containers.rooms);

  ofs.close();
  cout << "Data saved" << endl;
}

void save_persons(ofstream& ofs, const Person_c& persons_c)
{
  ofs << persons_c.size() << endl;
  for (auto person_ptr : persons_c) {
    person_ptr->save(ofs);
  }
}

void save_rooms(ofstream& ofs, const Room_c& rooms_c)
{
  ofs << rooms_c.size() << endl;
  for_each(rooms_c.begin(),
           rooms_c.end(),
           bind(&Room::save, bind(&Room_c::value_type::second, _1), ref(ofs)));
}

void print_bad_command_and_clear() {
  cout << "Unrecognized command!" << endl;
  while (cin.get() != '\n');
}

Meeting* read_and_find_meeting(Room_c& rooms_c)
{
  Room& room = read_and_find_room(rooms_c);
  int time = read_time();
  return room.get_Meeting(time);
}

Person* read_and_find_person(const Person_c& persons_c)
{
  string lastname;
  cin >> lastname;
  Person probe {lastname};
  Person_c::iterator person_it;
  if ((person_it = persons_c.find(&probe)) == persons_c.end()) {
    throw Error{"No person with that name!"};
  }
  return *person_it;
}

Room& read_and_find_room(Room_c& rooms_c)
{
  int room_number = read_room_number();
  auto room_it = rooms_c.find(room_number);
  if (room_it == rooms_c.end()) {
    throw Error{"No room with that number!"};
  }
 
  return room_it->second;
}

int read_int()
{
  int num;
  if (!(cin >> num)) {
    throw Error{"Could not read an integer value!"};
  }
  return num;
}

int read_int_from_file(std::ifstream& ifs)
{
  int num;
  if (!(ifs >> num)) {
    throw_invalid_data();
  }
  return num;
}

int read_room_number()
{
  int num = read_int();
  if (num <= 0) {
    throw Error{"Room number is not in range!"};
  }
  return num;
}

int read_time()
{
  int time = read_int();
  if (time < 1 || time > 12 || (time > 5 && time < 9)) {
    throw Error{"Time is not in range!"};
  }
  return time;
}

