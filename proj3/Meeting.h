#ifndef MEETING_H_
#define MEETING_H_

/* Meeting class - this class represents a Meeting in terms of a time, topic, and 
list of participants. 
Note: Meeting's interface and I/O functions assume that the time is expressed in 12-hour format.

Public functions provide for maintaining the list of participants,
but no direct access to it is allowed. 

An overloaded operator< is provided to simplify constructing a list of Meetings 
in order by time, and an output operator to simplify printing the Meeting information.

We let the compiler supply the destructor and the copy/move constructors and assignment operators.

*/
#include "Utility.h"

#include <iosfwd>
#include <list>
#include <set>
#include <string>

class Person;
class Room;
class Meeting {
  using Participants_c = std::list<Person*>;
  using Person_c = std::set<Person*, Person_comp>;
  public:

	Meeting(int time_, const std::string& topic_) : time{time_}, topic{topic_} {}

	// construct a Meeting with only a time
	Meeting(int time_) : time{time_} {}

  // only for letting Person's know that they no longer have commitments
  ~Meeting() noexcept;

	// Construct a Meeting from an input file stream in save format
	// Throw Error exception if invalid data discovered in file.
	// No check made for whether the Meeting already exists or not.
	// Person list is needed to resolve references to meeting participants
	// Input for a member variable value is read directly into the member variable.
	Meeting(std::ifstream& is, const Person_c& persons_c, const Room& room);

	// accessors
	int get_time() const
		{ return time; }

  std::string get_topic() const
    { return topic; }

	// Meeting objects manage their own participant list. Participants
	// are identified by a pointer to that individual's Person object.

	// Add to the list, throw exception if participant was already there.
	void add_participant(Person* p, const Room& room);
	// Return true if the person is a participant, false if not.
	bool is_participant_present(const Person* p) const;
	// Remove from the list, throw exception if participant was not found.
	void remove_participant(Person* p);

  void transfer_participants(Meeting& new_meeting, const Room& new_room);

	// Write a Meeting's data to a stream in save format with final endl.
	void save(std::ostream& os) const;

	// This operator defines the order relation between meetings, based just on the time
	bool operator< (const Meeting& other) const;
	
	friend std::ostream& operator<< (std::ostream& os, const Meeting& meeting);
	
  private:
    int time;
    std::string topic;
	  Participants_c participants;
};

// Print the Meeting data as follows:
// The meeting time and topic on one line, followed either by:
// the no-participants message with an endl
// or the partipcants, one per line, with an endl after each one
std::ostream& operator<< (std::ostream& os, const Meeting& meeting);

#endif
