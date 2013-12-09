
#include "Model.h"

#include "AgentComponent.h"
#include "AgentIndividual.h"
#include "AgentGroup.h"
#include "AgentIndividual.h"
#include "Agent_factory.h"
#include "Geometry.h"
#include "Sim_object.h"
#include "Structure.h"
#include "Structure_factory.h"
#include "Utility.h"
#include "View.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>

using std::cout; using std::endl;
using std::string; using std::map; using std::vector;
using std::make_pair;
using std::shared_ptr; using std::unique_ptr;
using std::bind;
using std::min_element;
using namespace std::placeholders;

const int UNIQUE_STRING_PREFIX_SIZE = 2;

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

Model::Model() : time{0}, all_agents{unique_ptr<AgentComponent>()}
{
  all_agents = unique_ptr<AgentGroup>(new AgentGroup(""));
  
  insert_structure(create_structure("Rivendale", "Farm", Point(10., 10.)));
  insert_structure(create_structure("Sunnybrook", "Farm", Point(0., 30.)));
  insert_structure(create_structure("Shire", "Town_Hall", Point(20., 20.)));
  insert_structure(create_structure("Paduca", "Town_Hall", Point(30., 30.)));
  
  insert_new_agent(create_agent("Pippin", "Peasant", Point(5., 10.)));
  insert_new_agent(create_agent("Merry", "Peasant", Point(0., 25.)));
  insert_new_agent(create_agent("Zug", "Soldier", Point(20., 30.)));
  insert_new_agent(create_agent("Bug", "Soldier", Point(15., 20.)));
  insert_new_agent(create_agent("Iriel", "Archer", Point(20, 38.)));

}

// return singleton instance of model
Model& Model::get()
{
  static Model m;
  return m;
}

// is name already in use for either agent or structure?
// either the identical name, or identical in first two characters counts as in-use
bool Model::is_name_in_use(const string& name) const
{
  string prefix = name.substr(0,UNIQUE_STRING_PREFIX_SIZE);
  auto itr = objects.lower_bound(prefix);
  return itr != objects.end() && itr->first.find(prefix) == 0;
}

// check if the full name given matches an existing object
bool Model::object_fullname_exists(const std::string &name) const {
  auto itr = objects.find(name);
  return itr != objects.end();
}

// is there a structure with this name?
bool Model::is_structure_present(const string& name) const
{
  return structures.find(name) != structures.end();
}

void Model::insert_structure(shared_ptr<Structure>s)
{
  auto spair = make_pair(s->get_name(), s);
  objects.insert(spair);
  structures.insert(spair);
}

// add a new structure; assumes none with the same name
void Model::add_structure(shared_ptr<Structure> s)
{
  insert_structure(s);
  s->broadcast_current_state();
}

// will throw Error("Structure not found!") if no structure of that name
shared_ptr<Structure> Model::get_structure_ptr(const string& name) const
{
  if (!is_structure_present(name)) {
    throw Error("Structure not found!");
  }
  return structures.find(name)->second;
}

// returns the closest structure to the provided agent
shared_ptr<Structure> Model::closest_structure(shared_ptr<Sim_object> object) const
{
  // Check if the object arg is the only structure.
  if (structures.size() == 1 && structures.at(object->get_name()) == object) {
    return shared_ptr<Structure>();
  }
  
  auto closest_structure_it =
  min_element(structures.begin(),
              structures.end(),
              bind(sim_object_min_distance_comparator,
                   object,
                   bind(&Structures_t::value_type::second, _1),
                   bind(&Structures_t::value_type::second, _2)));
  
  return closest_structure_it->second;
}

// is there an agent with this name?
bool Model::is_agent_present(const string& name) const
{
  return all_agents->get_component(name) != nullptr; //TODO: that work?
}

void Model::insert_new_agent(shared_ptr<AgentIndividual> new_agent)
{
  all_agents->add_component(new_agent);
  objects[new_agent->get_name()] = new_agent; // TODO: error check for double add?
}

// add a new agent; assumes none with the same name
void Model::add_new_agent(shared_ptr<AgentIndividual> new_agent)
{
  insert_new_agent(new_agent);
  new_agent->broadcast_current_state();
}

//TODO: name check here and above?

// add a new group agent; assumes none with the same name
void Model::add_new_group(shared_ptr<AgentGroup> new_group)
{
  all_agents->add_component(new_group);
}

// remove an agent
void Model::remove_agent(shared_ptr<AgentComponent> agent)
{
  objects.erase(agent->get_name());
  all_agents->remove_component(agent->get_name());
}

// will throw Error("AgentComponent not found!") if no agent component of that name
shared_ptr<AgentComponent> Model::get_agent_ptr(const string& name) const
{
  if (!is_agent_present(name)) {
    throw Error("Agent not found!"); //TODO?
  }
  return all_agents->get_component(name);
}

// returns the closest agent to the provided object (but not the same agent)
shared_ptr<AgentComponent> Model::closest_agent(shared_ptr<Sim_object> object) const
{
 return all_agents->get_nearest(object);
  //NOTE!: need to included AgentIndividual cause of this...
}

shared_ptr<AgentComponent> Model::find_agents_in_range(shared_ptr<Sim_object> center, double range)
{
  return all_agents->get_all_in_range(center, range);
}

// tell all objects to describe themselves to the console
void Model::describe() const
{
  for (auto &i : objects)
    i.second->describe();
}

// increment the time, and tell all objects to update themselves
void Model::update()
{
  ++time;
  for (auto &i : objects)
    i.second->update();
}

/* View services */

// Attaching a View adds it to the container and causes it to be updated
// with all current objects'location (or other state information).
void Model::attach(shared_ptr<View> v)
{
  views.insert(v);
  for (auto &i : objects)
    i.second->broadcast_current_state();
}

// Detach the View by discarding the supplied pointer from the container of Views
// - no updates sent to it thereafter.
void Model::detach(shared_ptr<View> v)
{
  views.erase(v);
}

// notify the views about an object's location
void Model::notify_location(const string& name, Point location)
{
  for (auto &i : views)
    i->update_location(name, location);
}

// notify the views about an object's health
void Model::notify_health(const string& name, int health)
{
  for (auto &i : views)
    i->update_health(name, health);
}

// notify the views about an object's amount
void Model::notify_amount(const string& name, double amount)
{
  for (auto &i : views)
    i->update_amount(name, amount);
}

// notify the views that an object is now gone
void Model::notify_gone(const string& name)
{
  for (auto &i : views)
    i->update_remove(name);
}

// notify the views about an attack
void Model::notify_attack(const std::string& name, const std::string& target)
{
  for (auto &i : views)
    i->update_attack(name, target);
}

// notify the views about the end of an attack
void Model::notify_end_attack(const std::string &name)
{
  for (auto &i : views)
    i->update_end_attack(name);
}

