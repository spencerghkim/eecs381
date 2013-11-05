#include "Structure_factory.h"

#include "Farm.h"
#include "Geometry.h"
#include "Town_Hall.h"
#include "Utility.h"

#include <string>

#define FARM_TYPE_NAME "Farm"
#define TOWN_HALL_TYPE_NAME "Town_Hall"

Structure* create_structure(const std::string& name, const std::string& type, Point location)
{
  if (type == FARM_TYPE_NAME) {
    return new Farm(name, location);
  } else if (type == TOWN_HALL_TYPE_NAME) {
    return new Town_Hall(name, location);
  } else {
    throw Error{"Trying to create structure of unknown type!"};
  }
}
