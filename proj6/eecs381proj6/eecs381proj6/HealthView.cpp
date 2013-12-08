
#include "HealthView.h"

#include <iostream>

using std::cout; using std::endl;
using std::string;

const string HEALTH_VIEW_NAME = "Health";

// update the unit's health
void HealthView::update_health(const string& name, int health)
{
  update_value(name, health);
}

string HealthView::view_name()
{
  return HEALTH_VIEW_NAME;
}

