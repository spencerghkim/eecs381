#include "Agent_factory.h"

#include "Geometry.h"
#include "Peasant.h"
#include "Soldier.h"
#include "Utility.h"

#include <string>

#define PEASANT_TYPE_NAME "Peasant"
#define SOLDIER_TYPE_NAME "Soldier"

Agent* create_agent(const std::string& name, const std::string& type, Point location)
{
  if (type == PEASANT_TYPE_NAME) {
    return new Peasant(name, location);
  } else if (type == SOLDIER_TYPE_NAME) {
    return new Soldier(name, location);
  } else {
    throw Error{"Trying to create agent of unknown type!"};
  }
}

