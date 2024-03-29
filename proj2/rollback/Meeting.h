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
#include "Ordered_list.h"
#include "String.h"

#include <iosfwd>

class Person;
class Meeting {
  public:
	Meeting(int time_, const String& topic_) : time{time_}, topic{topic_} {}

	// construct a Meeting with only a time
	Meeting(int time_) : time{time_} {}

	// Construct a Meeting from an input file stream in save format
	// Throw Error exception if invalid data discovered in file.
	// No check made for whether the Meeting already exists or not.
	// Person list is needed to resolve references to meeting participants
	// Input for a member variable value is read directly into the member variable.
	Meeting(std::ifstream& is, const Ordered_list<const Person*, Less_than_ptr<const Person*> >& people);

	// accessors
	int get_time() const
		{ return time; }
	void set_time(int time_)
		{ time = time_; }

	// Meeting objects manage their own participant list. Participants
	// are identified by a pointer to that individual's Person object.

	// Add to the list, throw exception if participant was already there.
	void add_participant(const Person* p);
	// Return true if the person is a participant, false if not.
	bool is_participant_present(const Person* p) const;
	// Remove from the list, throw exception if participant was not found.
	void remove_participant(const Person* p);
			
	// Write a Meeting's data to a stream in save format with final endl.
	void save(std::ostream& os) const;

	// This operator defines the order relation between meetings, based just on the time
	bool operator< (const Meeting& other) const;
	
	friend std::ostream& operator<< (std::ostream& os, const Meeting& meeting);
	
  private:
    int time;
    String topic;
	using Participants_t = Ordered_list<const Person*, Less_than_ptr<const Person*>>;
	Participants_t participants;
};

// Print the Meeting data as follows:
// The meeting time and topic on one line, followed either by:
// the no-participants message with an endl
// or the partipcants, one per line, with an endl after each one
std::ostream& operator<< (std::ostream& os, const Meeting& meeting);

#endif
