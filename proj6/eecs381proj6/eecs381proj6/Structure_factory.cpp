#include "Structure_factory.h"

#include "Farm.h"
#include "Geometry.h"
#include "Town_Hall.h"
#include "Utility.h"

#include <string>

using std::string;
using std::shared_ptr;

const string FARM_TYPE_NAME = "Farm";
const string TOWN_HALL_TYPE_NAME = "Town_Hall";

shared_ptr<Structure> create_structure(const string& name,
                                       const string& type,
                                       Point location)
{
  if (type == FARM_TYPE_NAME) {
    return shared_ptr<Structure>(new Farm(name, location));
  } else if (type == TOWN_HALL_TYPE_NAME) {
    return shared_ptr<Structure>(new Town_Hall(name, location));
  } else {
    throw Error("Trying to create structure of unknown type!");
  }
}
