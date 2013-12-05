#include "Agent_factory.h"

#include "Geometry.h"
#include "Peasant.h"
#include "Warriors.h"
#include "Utility.h"

#include <string>

using std::string;
using std::shared_ptr;

const string ARCHER_TYPE_NAME = "Archer";
const string MAGICIAN_TYPE_NAME = "Magician";
const string PEASANT_TYPE_NAME = "Peasant";
const string SOLDIER_TYPE_NAME = "Soldier";


shared_ptr<Agent> create_agent(const string& name, const string& type, Point location)
{
  if (type == PEASANT_TYPE_NAME) {
    return shared_ptr<Agent>(new Peasant(name, location));
  } else if (type == SOLDIER_TYPE_NAME) {
    return shared_ptr<Agent>(new Soldier(name, location));
  } else if (type == ARCHER_TYPE_NAME) {
    return shared_ptr<Agent>(new Archer(name, location));
  } else if (type == MAGICIAN_TYPE_NAME ) {
    return shared_ptr<Agent>(new Magician(name, location));
  } else {
    throw Error{"Trying to create agent of unknown type!"};
  }
}

