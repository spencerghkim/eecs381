
#include "ListView.h"

#include "Utility.h"

#include <iostream>

using std::cout; using std::endl;
using std::string;

const string AMOUNTS_VIEW_NAME = "Amounts";
const string HEALTH_VIEW_NAME = "Health";

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

// Health View //

// update the unit's health
void HealthView::update_health(const string& name, int health)
{
  update_value(name, health);
}

string HealthView::view_name()
{
  return HEALTH_VIEW_NAME;
}

// Amounts View //

// update the amount of food
void AmountsView::update_amount(const string& name, double amount)
{
  update_value(name, amount);
}

string AmountsView::view_name()
{
  return AMOUNTS_VIEW_NAME;
}


