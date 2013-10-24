#ifndef PERSON_H_
#define PERSON_H_

#include <iosfwd>
#include <map>
#include <string>

class Room;
class Person {
  public:
    Person(const std::string& firstname_, const std::string& lastname_, const std::string& phoneno_)
      : lastname{lastname_}, firstname{firstname_}, phoneno{phoneno_} {}
    // construct a Person object with only a lastname
    Person(const std::string& lastname_)
      : lastname{lastname_} {}

    Person(const Person& p) = delete;
    Person(Person&& p) = delete;
    Person& operator= (const Person& p) = delete;
    Person& operator= (Person&& p) = delete;

    // Construct a Person object from a file stream in save format.
    // Throw Error exception if invalid data discovered in file.
    // No check made for whether the Person already exists or not.
    // Input for a member variable value is read directly into the member variable.
    Person(std::ifstream& is);
    
    // Accessors
    std::string get_lastname() const
      { return lastname; }

    // Add a commitment. Throw an exception if person is already committed
    // at the specified time.
    void add_commitment(int time, const Room& room);

    bool has_commitment(int time);

    // Check if the person has any commitments.
    bool has_commitments() const;

    void print_commitments() const;

    // Remove a commitment at the specified time.
    void remove_commitment(int time) noexcept;
	
    // Write a Person's data to a stream in save format with final endl.
    void save(std::ostream& os) const;

    // This operator defines the order relation between Persons, based just on the last name
    bool operator< (const Person& rhs) const
      { return lastname < rhs.lastname; }

    friend std::ostream& operator<< (std::ostream& os, const Person& person);

  private:
    std::string lastname;
    std::string firstname;
    std::string phoneno;

    using Commitment_c = std::map<int, const Room&>;
    Commitment_c commitments;
};

// output firstname, lastname, phoneno with one separating space, NO endl
std::ostream& operator<< (std::ostream& os, const Person& person);

#endif

