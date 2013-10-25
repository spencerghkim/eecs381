#ifndef MEETING_H_
#define MEETING_H_

/* Meeting class - this class represents a Meeting in terms of a time, topic, and 
list of participants. 
Note: Meeting's interface and I/O functions assume that the time is expressed in 12-hour format.

Public functions provide for maintaining the list of participants,
but no direct access to it is allowed. 

An overloaded operator< is provided to simplify constructing a list of Meetings 
in order by time, and an output operator to simplify printing the Meeting information.

*/
#include "Utility.h"

#include <iosfwd>
#include <set>
#include <string>
#include <vector>

class Person;
class Room;
class Meeting {
  using Participants_c = std::vector<Person*>;
  using Person_c = std::set<Person*, Person_ptr_comp>;
  public:
  
  // construct a Meeting with only a time
	Meeting(int time_) : time{time_} {}

  // construct a Meeting with a time and a topic
	Meeting(int time_, const std::string& topic_) : time{time_}, topic{topic_} {}

	// Construct a Meeting from an input file stream in save format
	// Throw Error exception if invalid data discovered in file.
	// No check made for whether the Meeting already exists or not.
	// Person list is needed to resolve references to meeting participants
	// Input for a member variable value is read directly into the member variable.
	Meeting(std::ifstream& is, const Person_c& persons_c, const Room* room);
  
  // Only for letting the people know that they no longer have a commitment at this time.
  ~Meeting();
  
  // Delete the assignment operators and copy/move assignment constructors. Meetings are
  // in charge of the integrity of commitments, so don't allow others to copy meetings.
  Meeting(const Meeting& m) = delete;
  Meeting(Meeting&& m) = delete;
  Meeting& operator= (const Meeting& m) = delete;
  Meeting& operator= (Meeting&& m) = delete;
  
	// accessors
	int get_time() const
		{ return time; }

  std::string get_topic() const
    { return topic; }

	// Meeting objects manage their own participant list. Participants
	// are identified by a pointer to that individual's Person object.

	// Add to the list, throw exception if participant was already there.
	void add_participant(Person* p, const Room* room);
	// Return true if the person is a participant, false if not.
	bool is_participant_present(const Person* p) const;
	// Remove from the list, throw exception if participant was not found.
	void remove_participant(Person* p);

  // Transfer participants from this meeting into a new Meeting.
  void transfer_participants(Meeting* new_meeting, const Room* new_room);
  
  // Clear all participants.
  void clear_participants();

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
