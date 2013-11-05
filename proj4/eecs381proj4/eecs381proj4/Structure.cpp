#include "Structure.h"

#include "Model.h"

#include <iostream>

Structure::~Structure ()
{
  std::cout << "Structure " << get_name() << " destructed" << std::endl;
}

void Structure::describe() const
{
  std::cout << get_name() << " at " << location << std::endl;
}

void Structure::broadcast_current_state()
{
  g_Model_ptr->notify_location(get_name(), location);
}
