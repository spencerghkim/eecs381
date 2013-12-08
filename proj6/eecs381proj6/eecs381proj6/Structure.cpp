#include "Structure.h"

#include "Model.h"

#include <iostream>

using std::string;
using std::cout; using std::endl;

Structure::Structure (const string& name_, Point location_) :
  Sim_object(name_), location{location_} {}

// Explicit default destructor.
Structure::~Structure () {}

void Structure::describe() const
{
  cout << get_name() << " at " << location << endl;
}

void Structure::broadcast_current_state()
{
  Model::get().notify_location(get_name(), location);
}
