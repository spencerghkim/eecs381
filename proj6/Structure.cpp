#include "Structure.h"

#include "Model.h"

#include <iostream>

Structure::Structure (const std::string& name_, Point location_)
    : Sim_object(name_), location{location_} {}

// Explicit default destructor.
Structure::~Structure () {}

void Structure::describe() const
{
  std::cout << get_name() << " at " << location << std::endl;
}

void Structure::broadcast_current_state()
{
  Model::get().notify_location(get_name(), location);
}
