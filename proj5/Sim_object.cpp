#include "Sim_object.h"

#include "Geometry.h"

#include <iostream>
#include <string>

Sim_object::Sim_object(const std::string& name_) : name{name_}
{
  std::cout << "Sim_object " << name << " constructed" << std::endl;
}

Sim_object::~Sim_object()
{
  std::cout << "Sim_object " << name << " destructed" << std::endl;
}
