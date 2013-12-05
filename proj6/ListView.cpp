
#include "ListView.h"

#include <iostream>

using std::cout; using std::endl;
using std::string;

// Base List View //
ListView::~ListView() {} // explicit dtor

// Notify for removal of object
void ListView::update_remove(const string& name)
{
  objects.erase(name);
}

// prints out the view
void ListView::draw()
{
  cout << "Current " << view_name() << ":" << endl;
  cout << "--------------" << endl;
  for (auto i : objects) {
    cout << i.first << ": " << i.second << endl;
  }
  cout << "--------------" << endl;
}

// discards the view's contents
void ListView::clear()
{
  objects.clear();
}

// update the generic value held by the ListView
void ListView::update_value(const string& name, double value)
{
  objects[name] = value;
}
