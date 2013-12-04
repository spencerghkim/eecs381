#include "Model.h"

#include "Agent.h"
#include "Agent_factory.h"
#include "Geometry.h"
#include "Sim_object.h"
#include "Structure.h"
#include "Structure_factory.h"
#include "Utility.h"
#include "Views.h"

#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

using std::map; using std::string; using std::set; using std::vector;
using std::shared_ptr;
using namespace std::placeholders;

using Sim_object_c = map<string, shared_ptr<Sim_object>>;
using Agent_c = map<string, shared_ptr<Agent>>;
using Structure_c = map<string, shared_ptr<Structure>>;
using View_c = set<shared_ptr<View>>;

// Returns a boolean indicating whether or not s1 is closer to origin than s2. We deal
// with self comparison (i.e. one of s1 or s2 is actually the origin object), by pushing
// origin to the end of the list.
bool sim_object_min_distance_comparator(const shared_ptr<Sim_object> origin,
                                        const shared_ptr<Sim_object> s1,
                                        const shared_ptr<Sim_object> s2)
{
  // If s1 and origin are the same object, s2 should always come first.
  if (s1 == origin) {
    return false;
  }
  
  // Similarly, if s2 and origin are the same object, s1 should always come first.
  if (s2 == origin) {
    return true;
  }
  
  return cartesian_distance(s1->get_location(), origin->get_location()) <
         cartesian_distance(s2->get_location(), origin->get_location());
}

Model& Model::getInstance() {
  static Model instance;
  return instance;
}

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
  add_agent_helper(create_agent("Iriel", "Archer", Point(20., 38.)));
}

// is name already in use for either agent or structure?
  // either the identical name, or identical in first two characters counts as in-use
bool Model::is_name_in_use(const std::string& name) const
{
  string prefix = name.substr(0, UNIQUE_STRING_PREFIX_SIZE);
  auto it = all_objects.lower_bound(prefix);
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

void Model::add_structure_helper(shared_ptr<Structure> structure)
{
  all_objects.insert(std::make_pair(structure->get_name(), structure));
  structures.insert(std::make_pair(structure->get_name(), structure));
}

// add a new structure; assumes none with the same name
void Model::add_structure(shared_ptr<Structure> structure)
{
  add_structure_helper(structure);
  structure->broadcast_current_state();
}

// will throw Error("Structure not found!") if no structure of that name
shared_ptr<Structure> Model::get_structure_ptr(const std::string& name) const
{
  Structure_c::const_iterator it = structures.find(name);
  if (it == structures.end()) {
    throw Error{"Structure not found!"};
  }
  return it->second;
}

shared_ptr<Structure> Model::find_closest_structure(shared_ptr<Sim_object> object) const
{
  // Check if the object arg is the only structure.
  if (structures.size() == 1 && structures.at(object->get_name()) == object) {
    return shared_ptr<Structure>();
  }
  
  auto closest_structure_it =
      std::min_element(structures.begin(),
                       structures.end(),
                       std::bind(sim_object_min_distance_comparator,
                                 object,
                                 std::bind(&Structure_c::value_type::second, _1),
                                 std::bind(&Structure_c::value_type::second, _2)));
  
  return closest_structure_it->second;
}

// is there an agent with this name?
bool Model::is_agent_present(const std::string& name) const
{
  return agents.find(name) != agents.end();
}

void Model::add_agent_helper(shared_ptr<Agent>agent)
{
  all_objects.insert(std::make_pair(agent->get_name(), agent));
  agents.insert(std::make_pair(agent->get_name(), agent));
}

// add a new agent; assumes none with the same name
void Model::add_agent(shared_ptr<Agent> agent)
{
  add_agent_helper(agent);
  agent->broadcast_current_state();
}

// will throw Error("Agent not found!") if no agent of that name
shared_ptr<Agent> Model::get_agent_ptr(const std::string& name) const
{
  Agent_c::const_iterator it = agents.find(name);
  if (it == agents.end()) {
    throw Error{"Agent not found!"};
  }
  return it->second;
}

void Model::remove_agent(shared_ptr<Agent> agent)
{
  all_objects.erase(agent->get_name());
  agents.erase(agent->get_name());
}

shared_ptr<Agent> Model::find_closest_agent(std::shared_ptr<Sim_object> object) const
{
  // Check if the object arg is the only agent we have.
  if (agents.size() == 1 && agents.at(object->get_name()) == object) {
    return shared_ptr<Agent>();
  }
  
  auto closest_agent_it =
      std::min_element(agents.begin(),
                       agents.end(),
                       std::bind(sim_object_min_distance_comparator,
                                 object,
                                 std::bind(&Agent_c::value_type::second, _1),
                                 std::bind(&Agent_c::value_type::second, _2)));
  
  return closest_agent_it->second;
}

vector<shared_ptr<Agent>> Model::find_agents_in_range(shared_ptr<Sim_object> center, double range)
{
  vector<shared_ptr<Agent>> agents_in_range;
  
  for (Agent_c::value_type agent_pair : agents) {
    if (agent_pair.second != center &&
        cartesian_distance(agent_pair.second->get_location(), center->get_location()) < range) {
      agents_in_range.push_back(agent_pair.second);
    }
  }
  
  return agents_in_range;
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
}

// Attaching a View adds it to the container and causes it to be updated
  // with all current objects'location (or other state information.
void Model::attach(shared_ptr<View> view)
{
  views.insert(view);
  std::for_each(all_objects.begin(), all_objects.end(), [] (Sim_object_c::value_type pair) {
      pair.second->broadcast_current_state();
  });
}

// Detach the View by discarding the supplied pointer from the container of Views
void Model::detach(shared_ptr<View> view)
{
  views.erase(view);
}

void Model::notify_amount(const std::string &name, double amount)
{
  std::for_each(views.begin(), views.end(), bind(&View::update_amount, _1, name, amount));
}

void Model::notify_health(const std::string &name, int health)
{
  std::for_each(views.begin(), views.end(), bind(&View::update_health, _1, name, health));
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

