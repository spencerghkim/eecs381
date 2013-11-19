#include "Model.h"

#include "Agent.h"
#include "Agent_factory.h"
#include "Geometry.h"
#include "Sim_object.h"
#include "Structure.h"
#include "Structure_factory.h"
#include "Utility.h"
#include "View.h"

#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <string>

using std::map; using std::string; using std::set;
using namespace std::placeholders;

using Sim_object_c = map<string, Sim_object *>;
using Agent_c = map<string, Agent *>;
using Structure_c = map<string, Structure *>;
using View_c = set<View *>;

/* Here define g_Model_ptr */
Model* g_Model_ptr = nullptr;

/* *** create the initial structures and agents using the following calls to the factories
in the Model constructor. You will have to write code to save the returned pointers. 
Do not change the order of these calls. You should delete this comment. */
Model::Model()
{
	add_structure_helper(create_structure("Rivendale", "Farm", Point(10., 10.)));
	add_structure_helper(create_structure("Sunnybrook", "Farm", Point(0., 30.)));
	add_structure_helper(create_structure("Shire", "Town_Hall", Point(20., 20.)));
	add_structure_helper(create_structure("Paduca", "Town_Hall", Point(30., 30.)));
	
	add_agent_helper(create_agent("Pippin", "Peasant", Point(5., 10.)));
  add_agent_helper(create_agent("Merry", "Peasant", Point(0., 25.)));
	add_agent_helper(create_agent("Zug", "Soldier", Point(20., 30.)));
	add_agent_helper(create_agent("Bug", "Soldier", Point(15., 20.)));
}

Model::~Model()
{
  std::for_each(all_objects.begin(), all_objects.end(), [] (Sim_object_c::value_type pair) {
      delete pair.second;
  });
}


// is name already in use for either agent or structure?
  // either the identical name, or identical in first two characters counts as in-use
bool Model::is_name_in_use(const std::string& name) const
{
  string prefix = name.substr(0, UNIQUE_STRING_PREFIX_SIZE);
  Sim_object_c::const_iterator it = all_objects.lower_bound(prefix);
  if (it != all_objects.end() &&
      it->first.substr(0, UNIQUE_STRING_PREFIX_SIZE) == prefix) {
    return true;
  } else {
    return false;
  }
}

bool Model::is_structure_present(const std::string& name) const
{
  return structures.find(name) != structures.end();
}

void Model::add_structure_helper(Structure *structure)
{
  all_objects.insert(std::make_pair(structure->get_name(), structure));
  structures.insert(std::make_pair(structure->get_name(), structure));
}

// add a new structure; assumes none with the same name
void Model::add_structure(Structure* structure)
{
  add_structure_helper(structure);
  structure->broadcast_current_state();
}

// will throw Error("Structure not found!") if no structure of that name
Structure* Model::get_structure_ptr(const std::string& name) const
{
  Structure_c::const_iterator it = structures.find(name);
  if (it == structures.end()) {
    throw Error{"Structure not found!"};
  }
  return it->second;
}

// is there an agent with this name?
bool Model::is_agent_present(const std::string& name) const
{
  return agents.find(name) != agents.end();
}

void Model::add_agent_helper(Agent *agent)
{
  all_objects.insert(std::make_pair(agent->get_name(), agent));
  agents.insert(std::make_pair(agent->get_name(), agent));
}

// add a new agent; assumes none with the same name
void Model::add_agent(Agent* agent)
{
  add_agent_helper(agent);
  agent->broadcast_current_state();
}

// will throw Error("Agent not found!") if no agent of that name
Agent* Model::get_agent_ptr(const std::string& name) const
{
  Agent_c::const_iterator it = agents.find(name);
  if (it == agents.end()) {
    throw Error{"Agent not found!"};
  }
  return it->second;
}

// tell all objects to describe themselves to the console
void Model::describe() const
{
  std::for_each(all_objects.begin(), all_objects.end(), [] (Sim_object_c::value_type pair) {
      pair.second->describe();
  });
}

// increment the time, and tell all objects to update themselves
void Model::update()
{
  ++time;
  std::for_each(all_objects.begin(), all_objects.end(), [] (Sim_object_c::value_type pair) {
      pair.second->update();
  });
  
  // Delete all disappearing agents and remove them from both containers.
  for (Agent_c::iterator it = agents.begin(); it != agents.end(); /* empty */) {
    if (it->second->is_disappearing()) {
      all_objects.erase(it->second->get_name());
      delete it->second;
      agents.erase(it++);
    } else {
      ++it;
    }
  }
}

// Attaching a View adds it to the container and causes it to be updated
  // with all current objects'location (or other state information.
void Model::attach(View* view)
{
  views.insert(view);
  std::for_each(all_objects.begin(), all_objects.end(), [] (Sim_object_c::value_type pair) {
    pair.second->broadcast_current_state();
  });
}

// Detach the View by discarding the supplied pointer from the container of Views
  // - no updates sent to it thereafter.
void Model::detach(View* view)
{
  views.erase(view);
}

void Model::notify_location(const std::string& name, Point location)
{
  std::for_each(views.begin(), views.end(), bind(&View::update_location, _1, name, location) );
}

// notify the views that an object is now gone
void Model::notify_gone(const std::string& name)
{
  std::for_each(views.begin(), views.end(), bind(&View::update_remove, _1, name) );
}

