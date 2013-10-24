#include "Ordered_list.h"  // must not #include any project-specific modules at all!

#include "Meeting.h" // should incompletely declare class Person

//#include "Person.h" // do you need to include this to make it compile?


#include <iostream>

using namespace std;


int main(void)
{
  // declare a list of Meeting OBJECTS
  Ordered_list<Meeting> meetings;
  // does adding the following line make the compile fail?
  Ordered_list<Meeting> meetings2(meetings);

  Meeting m1(10, "Meeting at 10");
  // does adding the following line make the compile fail?
  meetings.insert(m1);

  cout << "Done!" << endl;

  return 0;
}
