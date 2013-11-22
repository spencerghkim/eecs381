#include "Town_Hall.h"

#include "Geometry.h"
#include "Model.h"

#include <iostream>
#include <string>

#define TAX_RATE 0.1
#define MINIMUM_FOOD_AMOUNT 1.0

Town_Hall::Town_Hall(const std::string& in_name, Point in_location)
    : Structure(in_name, in_location), total_food{0} {}

void Town_Hall::deposit(double deposit_amount)
{
  total_food += deposit_amount;
  Model::getInstance().notify_amount(get_name(), total_food);
}

double Town_Hall::withdraw(double amount_to_obtain)
{
  double withdrawl = amount_to_obtain;
  double max_withdrawl = total_food - (total_food * TAX_RATE);
  
  // Check that we aren't dealing with miniscule amounts of food.
  if (max_withdrawl < MINIMUM_FOOD_AMOUNT) {
    max_withdrawl = 0.0;
  }
  
  // Check whether or not we can return the amount requested.
  if (amount_to_obtain > max_withdrawl) {
    withdrawl = max_withdrawl;
  }
  
  total_food -= withdrawl;
  Model::getInstance().notify_amount(get_name(), total_food);
  return withdrawl;
}

void Town_Hall::describe() const {
  std::cout << "Town_Hall ";
  Structure::describe();
  std::cout << "   Contains " << total_food << std::endl;
}
  
void Town_Hall::broadcast_current_state()
{
  Structure::broadcast_current_state();
  Model::getInstance().notify_amount(get_name(), total_food);
}
