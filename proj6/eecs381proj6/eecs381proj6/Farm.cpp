#include "Farm.h"

#include "Geometry.h"
#include "Model.h"

#include <iostream>

using std::string;
using std::cout; using std::endl;

const double INITIAL_FOOD = 50.0;
const double FOOD_PRODUCED_PER_UPDATE = 2.0;

Farm::Farm(const string& in_name, Point in_location) :
  Structure{in_name, in_location}, food_available{INITIAL_FOOD} {}

double Farm::withdraw(double amount_to_get)
{
  double withdrawl = amount_to_get;
  if (amount_to_get > food_available) {
    withdrawl = food_available;
  }
  
  food_available -= withdrawl;
  
  // Notify the model that some food has been withdrawn.
  Model::get().notify_amount(get_name(), food_available);
  return withdrawl;
}

void Farm::update()
{
  food_available += FOOD_PRODUCED_PER_UPDATE;
  Model::get().notify_amount(get_name(), food_available);
  cout << "Farm " << get_name() << " now has " << food_available << endl;
}

void Farm::describe() const
{
  cout << "Farm ";
  Structure::describe();
  cout << "   Food available: " << food_available << endl;
}

void Farm::broadcast_current_state()
{
  Structure::broadcast_current_state();
  Model::get().notify_amount(get_name(), food_available);
}
