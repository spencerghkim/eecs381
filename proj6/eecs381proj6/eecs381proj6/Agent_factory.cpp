#include "Agent_factory.h"

#include "Geometry.h"
#include "Peasant.h"
#include "Warrior.h"
#include "Soldier.h"
#include "Archer.h"
#include "Magician.h"
#include "Utility.h"

#include <memory>
#include <string>

using std::string;
using std::shared_ptr;
using std::make_shared;

const string ARCHER_TYPE_NAME = "Archer";
const string MAGICIAN_TYPE_NAME = "Magician";
const string PEASANT_TYPE_NAME = "Peasant";
const string SOLDIER_TYPE_NAME = "Soldier";

shared_ptr<AgentComponent> create_agent(const string& name, const string& type, Point location)
{
  if (type == PEASANT_TYPE_NAME) {
    return make_shared<Peasant>(name,location);
  } else if (type == SOLDIER_TYPE_NAME) {
    return make_shared<Soldier>(name, location);
  } else if (type == ARCHER_TYPE_NAME) {
    return make_shared<Archer>(name, location);
  } else if (type == MAGICIAN_TYPE_NAME ) {
    return make_shared<Magician>(name, location);
  } else {
    throw Error("Trying to create agent of unknown type!");
  }
}

