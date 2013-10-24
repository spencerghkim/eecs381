#include "Meeting.h"
#include "Room.h"
#include "Ordered_list.h"
#include "Person.h"
#include "p2_globals.h"
#include "Utility.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <new>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

using Room_list = Ordered_list<Room>;
using Person_list = Ordered_list<const Person *, Less_than_ptr<const Person* > >;
using Person_it = Person_list::Iterator;
using Room_it = Room_list::Iterator;

void add_meeting(Room_list& rooms_c);
void add_participant(Room_list& rooms_c, const Person_list& persons_c);
void add_person(Person_list& persons_c);
void add_room(Room_list& rooms_c);

void delete_person(const Room_list& rooms_c, Person_list& persons_c);
void delete_room(Room_list& rooms_c);
void delete_room_all(Room_list& rooms_c);
void delete_meeting(Room_list& rooms_c);
void delete_participant(Room_list& rooms_c, const Person_list& persons_c);
void delete_meeting_all(Room_list& rooms_c);
void delete_person_all(const Room_list& rooms_c, Person_list& persons_c);
void delete_all(Room_list& rooms_c, Person_list& persons_c);

void load_data(Room_list& rooms_c, Person_list& persons_c);
Person_list load_persons(std::ifstream& is);
Room_list load_rooms(std::ifstream& is, const Person_list& persons_c);

void print_meeting(const Room_list& rooms_c);
void print_meeting_all(const Room_list& rooms_c);
void print_memory(const Room_list& rooms_c, const Person_list& persons_c);
void print_person(const Person_list& persons_c);
void print_person_all(const Person_list& persons_c);
void print_room(const Room_list& rooms_c);

void reschedule_meeting(const Room_list& rooms_c);

void save_data(const Room_list& rooms_c, const Person_list& persons_c);
void save_persons(std::ofstream& ofs, const Person_list& persons_c);
void save_rooms(std::ofstream& ofs, const Room_list& rooms_c);


// Utility functions in p2_main.cpp

void free_person_ptr(const Person* person_ptr);

void print_bad_command_and_clear();

Person* read_and_create_person(const Person_list& persons_c);
Room read_and_create_room(const Room_list& rooms_c);
Meeting& read_and_find_meeting(const Room_list& rooms_c);
Person_it read_and_find_person(const Person_list& persons_c);
Room_it read_and_find_room(const Room_list& rooms_c);
int read_int();
int read_int_from_file(std::ifstream& ifs);
int read_room_number();
int read_time();


int main() {
  Room_list rooms_c {};
  Person_list persons_c {};

  char first_letter;
  char second_letter;

  while (1) {
    cout << "\nEnter command: ";
    cin >> first_letter >> second_letter;

    try {

    switch (first_letter) {
      case 'a': {
        switch (second_letter) {
          case 'i':
            add_person(persons_c);
            break;
          case 'r':
            add_room(rooms_c);
            break;
          case 'm':
            add_meeting(rooms_c);
            break;
          case 'p':
            add_participant(rooms_c, persons_c);
            break;
          default:
            print_bad_command_and_clear();
            break;
        }
        break;
      }
      case 'd': {
        switch (second_letter) {
          case 'i':
            delete_person(rooms_c, persons_c);
            break;
          case 'r':
            delete_room(rooms_c);
            break;
          case 'm':
            delete_meeting(rooms_c);
            break;
          case 'p':
            delete_participant(rooms_c, persons_c);
            break;
          case 's':
            delete_meeting_all(rooms_c);
            break;

          case 'g':
            delete_person_all(rooms_c, persons_c);
            break;
          case 'a':
            delete_all(rooms_c, persons_c);
            break;
          default:
            print_bad_command_and_clear();
            break;
        }
        break;
      }
      case 'l': {
        switch (second_letter) {
          case 'd':
            load_data(rooms_c, persons_c);
            break;
          default:
            print_bad_command_and_clear();
            break;
        }
        break;
      }
      case 'p': {
        switch (second_letter) {
          case 'i':
            print_person(persons_c);
            break;
          case 'r':
            print_room(rooms_c);
            break;
          case 'm':
            print_meeting(rooms_c);
            break;
          case 's':
            print_meeting_all(rooms_c);
            break;
          case 'g':
            print_person_all(persons_c);
            break;
          case 'a':
            print_memory(rooms_c, persons_c);
            break;
          default:
            print_bad_command_and_clear();
            break;
        }
        break;
      }
      case 'q': {
        switch (second_letter) {
          case 'q':
            delete_all(rooms_c, persons_c);
            cout << "Done" << endl;
            return 0;
          default:
            print_bad_command_and_clear();
            break;
        }
        break;
      }
      case 'r': {
        switch (second_letter) {
          case 'm':
            reschedule_meeting(rooms_c);
            break;
          default:
            print_bad_command_and_clear();
            break;
        }
        break;
      }
      case 's': {
        switch (second_letter) {
          case 'd':
            save_data(rooms_c, persons_c);
            break;
          default:
            print_bad_command_and_clear();
            break;
        }
        break;
      }
      default:
        print_bad_command_and_clear();
        break;
    }

    } catch (Error& e) {
      cin.clear();
      while (cin.get() != '\n');
      cout << e.msg << endl;
    } catch (std::bad_alloc& bad_alloc) {
      cerr << "Memory allocation threw bad allocation!" << endl;
      delete_all(rooms_c, persons_c);
      return 0;
    } catch (String_exception& string_ex) {
      cerr << string_ex.msg << endl;
      delete_all(rooms_c, persons_c);
      return 0;
    } catch (...) {
      cerr << "Unknown exception!" << endl;
      delete_all(rooms_c, persons_c);
      return 0;
    }
  }

  return 0;
}

void add_meeting(Room_list& rooms_c)
{
  auto room_it = read_and_find_room(rooms_c);
  int time = read_time();
  String topic;
  cin >> topic;
  Meeting new_meeting(time, topic);
  room_it->add_Meeting(new_meeting);

  cout << "Meeting added at " << time << endl;
}

void add_participant(Room_list& rooms_c, const Person_list& persons_c) 
{
  Meeting& meeting = read_and_find_meeting(rooms_c);
  auto person_it = read_and_find_person(persons_c);
  meeting.add_participant(*person_it);

  cout << "Participant " << (*person_it)->get_lastname() << " added" << endl;
}

void add_person(Person_list& persons_c)
{
  Person* person_ptr = read_and_create_person(persons_c);
  persons_c.insert(person_ptr);

  cout << "Person " << person_ptr->get_lastname() << " added" << endl;
}

void add_room(Room_list& rooms_c)
{
  Room room = read_and_create_room(rooms_c);
  rooms_c.insert(room);

  cout << "Room " << room.get_room_number() << " added" << endl;
}

void delete_person(const Room_list& rooms_c, Person_list& persons_c)
{
  auto person_it = read_and_find_person(persons_c);
  for (auto& room : rooms_c) {
    if (room.is_participant_present(*person_it)) {
      throw Error{"This person is a participant in a meeting!"};
    }
  }

  cout << "Person " << (*person_it)->get_lastname() << " deleted" << endl;

  delete *person_it;
  persons_c.erase(person_it);
}

void delete_room(Room_list& rooms_c)
{
  auto room_it = read_and_find_room(rooms_c);

  cout << "Room " << room_it->get_room_number() << " deleted" << endl;
  rooms_c.erase(room_it);
}

void delete_room_all(Room_list& rooms_c)
{
  rooms_c.clear();
  cout << "All rooms and meetings deleted" << endl;
}

void delete_meeting(Room_list& rooms_c)
{
  auto room_it = read_and_find_room(rooms_c);
  int time = read_time();
  room_it->remove_Meeting(time);
 
  cout << "Meeting at " << time << " deleted" << endl;
}

void delete_participant(Room_list& rooms_c, const Person_list& persons_c)
{
  Meeting& meeting = read_and_find_meeting(rooms_c);
  auto person_it = read_and_find_person(persons_c);
  meeting.remove_participant(*person_it);

  cout << "Participant " << (*person_it)->get_lastname() << " deleted" << endl;
}

void delete_meeting_all(Room_list& rooms_c)
{
  for (auto& room : rooms_c) {
    room.clear_Meetings();
  }
  cout << "All meetings deleted" << endl;
}

void delete_person_all(const Room_list& rooms_c, Person_list& persons_c)
{
  for (auto& room : rooms_c) {
    if (room.has_Meetings()) {
      throw Error{"Cannot clear people list unless there are no meetings!"};
    }
  }

  // Before clearing the people, we need to go through and free the pointers.
  apply(persons_c.begin(), persons_c.end(), free_person_ptr);
  persons_c.clear();

  cout << "All persons deleted" << endl;
}

void delete_all(Room_list& rooms_c, Person_list& persons_c)
{
  delete_room_all(rooms_c);
  delete_person_all(rooms_c, persons_c);
}

void load_data(Room_list& rooms_c, Person_list& persons_c)
{
  String filename;
  cin >> filename;
  std::ifstream ifs(filename.c_str(), std::ifstream::in);
  
  if (!ifs.is_open()) {
    throw Error{"Could not open file!"};
  }

  // We need this inner try block in order to catch any errors and restore the old
  // copies of Rooms and Persons.
  Person_list persons_backup {persons_c};
  Room_list rooms_backup {rooms_c};
  persons_c.clear();
  rooms_c.clear();
  try {
    persons_c = load_persons(ifs);
    rooms_c = load_rooms(ifs, persons_c);

  } catch (Error& e) {
    // Make sure to actually delete any added persons - everything else will be taken care
    // of when we reassign the containers to their backups.
    apply(persons_c.begin(), persons_c.end(), free_person_ptr);

    persons_c = persons_backup;
    rooms_c = rooms_backup;

    ifs.close();
    throw e;
  }

  // Load was successful! Free any person ptr in the backup copies.
  apply(persons_backup.begin(), persons_backup.end(), free_person_ptr);

  ifs.close();
  cout << "Data loaded" << endl;
}

Person_list load_persons(std::ifstream& ifs)
{
  Person_list persons_c {};
  for (int num_persons = read_int_from_file(ifs); num_persons > 0; --num_persons) {
    Person* new_person = new Person(ifs);
    persons_c.insert(new_person);
  }
  return persons_c;
}

Room_list load_rooms(std::ifstream& ifs, const Person_list& persons_c)
{
  Room_list rooms_c {};
  for (int num_rooms = read_int_from_file(ifs); num_rooms > 0; --num_rooms) {
    Room new_room(ifs, persons_c);
    rooms_c.insert(new_room);
  }
  return rooms_c;
} 

void print_meeting(const Room_list& rooms_c)
{
  Meeting& meeting = read_and_find_meeting(rooms_c);
  cout << meeting;
}
 
void print_meeting_all(const Room_list& rooms_c)
{
  if (rooms_c.empty()) {
    cout << "List of rooms is empty" << endl;
    return;
  }

  cout << "Information for " << rooms_c.size() << " rooms:" << endl;
  for (auto& room : rooms_c) {
    cout << room;
  }
}

void print_memory(const Room_list& rooms_c,
                  const Person_list& persons_c)
{
  cout << "Memory allocations:" << endl;
  cout << "Strings: " << String::get_number();
  cout << " with " << String::get_total_allocation() <<  " bytes total" << endl;
  cout << "Persons: " << persons_c.size() << endl;

  // Sum up number of meetings.
  int num_meetings = 0;
  for (auto& room : rooms_c) {
    num_meetings += room.get_number_Meetings();
  }
  cout << "Meetings: " << num_meetings << endl;
  cout << "Rooms: " << rooms_c.size() << endl;
  cout << "Lists: " << g_Ordered_list_count << endl;
  cout << "List Nodes: " << g_Ordered_list_Node_count << endl;
}

void print_person(const Person_list& persons_c)
{
  auto person_it = read_and_find_person(persons_c);
  cout << **person_it << endl;
}

void print_person_all(const Person_list& persons_c)
{
  if (persons_c.empty()) {
    cout << "List of people is empty" << endl;
  } else {
    cout << "Information for " << persons_c.size() << " people:" << endl;
    for (auto person_ptr : persons_c) {
      cout << *person_ptr << endl;
    }
  }
}

void print_room(const Room_list& rooms_c)
{
  auto room_it = read_and_find_room(rooms_c);
  cout << *room_it;
}

void reschedule_meeting(const Room_list& rooms_c)
{
  auto old_room_it = read_and_find_room(rooms_c);
  int old_time = read_time();
  Meeting old_Meeting = old_room_it->get_Meeting(old_time);

  auto new_room_it = read_and_find_room(rooms_c);
  int new_time = read_time();

  Meeting new_meeting {old_Meeting};
  new_meeting.set_time(new_time);

  // Actually insert the new meeting. Will throw if already exists.
  new_room_it->add_Meeting(new_meeting);

  // If we get here, we're safe to remove the meeting from the old room.
  old_room_it->remove_Meeting(old_time);

  cout << "Meeting rescheduled to room " << new_room_it->get_room_number();
  cout << " at " << new_time << endl;
}

void save_data(const Room_list& rooms_c, const Person_list& persons_c)
{
  String filename;
  cin >> filename;
  std::ofstream ofs(filename.c_str(), std::ifstream::out);
  
  if (!ofs.is_open()) {
    throw Error{"Could not open file!"};
  }

  save_persons(ofs, persons_c);
  save_rooms(ofs, rooms_c);

  ofs.close();
  cout << "Data saved" << endl;
}

void save_persons(std::ofstream& ofs, const Person_list& persons_c)
{
  ofs << persons_c.size() << endl;
  for (auto person_ptr : persons_c) {
    person_ptr->save(ofs);
  }
}

void save_rooms(std::ofstream& ofs, const Room_list& rooms_c)
{
  ofs << rooms_c.size() << endl;
  for (auto& room : rooms_c) {
    room.save(ofs);
  }
}

// Used as a function to apply on an Ordered_list.
void free_person_ptr(const Person* person_ptr)
{
  delete person_ptr;
}

void print_bad_command_and_clear() {
  cout << "Unrecognized command!" << endl;
  while (cin.get() != '\n');
}

Person* read_and_create_person(const Person_list& persons_c)
{
  String firstname;
  String lastname;
  String phoneno;
  cin >> firstname >> lastname >> phoneno;

  Person probe {lastname};
  if (persons_c.find(&probe) != persons_c.end()) {
    throw Error{"There is already a person with this last name!"};
  }
  Person* new_person = new Person(firstname, lastname, phoneno);

  return new_person;
}

Room read_and_create_room(const Room_list& rooms_c)
{
  int room_number = read_room_number();
  Room room {room_number};
  if (rooms_c.find(room) != rooms_c.end()) {
    throw Error{"There is already a room with this number!"};
  }
  return room;
}

Meeting& read_and_find_meeting(const Room_list& rooms_c)
{
  auto room_it = read_and_find_room(rooms_c);
  int time = read_time();
  return room_it->get_Meeting(time);
}

Person_it read_and_find_person(const Person_list& persons_c)
{
  String lastname;
  cin >> lastname;
  Person probe {lastname};
  Person_it person_it;
  if ((person_it = persons_c.find(&probe)) == persons_c.end()) {
    throw Error{"No person with that name!"};
  }
  return person_it;
}

Room_it read_and_find_room(const Room_list& rooms_c)
{
  int room_number = read_room_number();
  Room probe {room_number};
  Room_it room_it;
  if ((room_it = rooms_c.find(probe)) == rooms_c.end()) {
    throw Error{"No room with that number!"};
  }
  return room_it;
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

