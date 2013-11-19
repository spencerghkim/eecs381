#include "Farm.h"

#include "Geometry.h"

#include <iostream>

#define INITIAL_FOOD 50.0
#define FOOD_PRODUCED_PER_UPDATE 2.0

Farm::Farm(const std::string& in_name, Point in_location)
    : Structure(in_name, in_location), food_available{INITIAL_FOOD}
{
  std::cout << "Farm " << in_name << " constructed" << std::endl;
}

Farm::~Farm()
{
  std::cout << "Farm " << get_name() << " destructed" << std::endl;
}

double Farm::withdraw(double amount_to_get)
{
  double withdrawl = amount_to_get;
  if (amount_to_get > food_available) {
    withdrawl = food_available;
  }
  
  food_available -= withdrawl;
  return withdrawl;
}

void Farm::update()
{
  food_available += FOOD_PRODUCED_PER_UPDATE;
  std::cout << "Farm " << get_name() << " now has " << food_available << std::endl;
}

void Farm::describe() const
{
  std::cout << "Farm ";
  Structure::describe();
  std::cout << "   Food available: " << food_available << std::endl;
}
