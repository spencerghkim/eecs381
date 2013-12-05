
#include "AmountsView.h"

#include <iostream>

using std::cout; using std::endl;
using std::string;

const string AMOUNTS_VIEW_NAME = "Amounts";

// update the amount of food
void AmountsView::update_amount(const string& name, double amount)
{
  update_value(name, amount);
}

string AmountsView::view_name()
{
  return AMOUNTS_VIEW_NAME;
}


