
#include "Model.h"

#include "AgentComponent.h"
#include "AgentIndividual.h"
#include "AgentGroup.h"
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
using std::make_pair; using std::make_shared;
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

Model::Model() : time{0}
{
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

// is name already in use for either an AgentComponent or a Structure?
// either the identical name, or identical in first two characters counts as in-use

bool Model::is_name_in_use(const string& name) const
{
  string prefix = name.substr(0, UNIQUE_STRING_PREFIX_SIZE);
  
  // Check if the name is used by an object.
  auto itr = objects.lower_bound(prefix);
  if (itr != objects.end() && itr->first.find(prefix) == 0) {
    return true;
  }
  
  // Check to see if the name is used by a group.
  for (auto& component : agent_components) {
    if (component.second->has_prefix(prefix)) {
      return true;
    }
  }
  return false;
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
bool Model::is_agent_component_present(const string& name) const
{
  for (auto& component : agent_components) {
    if (component.second->get_component(name)) {
      return true;
    }
  }
  return false;
}

// is this agent component in a group?
bool Model::is_agent_component_in_group(std::shared_ptr<AgentComponent> comp) const
{
  // Loop through all of our top level components. If we don't find the component, it must
  // be inside a group.
  for (auto& component : agent_components) {
    if (component.second->get_name() == comp->get_name()) {
      return false;
    }
  }
  return true;
}

// are these two agent components in the same group?
bool Model::are_in_same_group(const string &a1, const string &a2) const
{
  for (auto& component : agent_components) {
    if (component.second->get_component(a1) &&
        component.second->get_component(a2)) {
      return true;
    }
  }
  return false;
}

// add agent individual, throws if name is already in use
void Model::add_new_agent(shared_ptr<AgentIndividual> new_agent)
{
  insert_new_agent(new_agent);
  new_agent->broadcast_current_state();
}

void Model::add_existing_agent_component(shared_ptr<AgentComponent> component)
{
  agent_components[component->get_name()] = component;
}

// add a new agent component, does nothing with sim_objects
void Model::add_new_agent_component(shared_ptr<AgentComponent> component)
{
  if (is_name_in_use(component->get_name())) {
    throw Error("Name is already in use!");
  }
  agent_components[component->get_name()] = component;
}

// adds existing component to existing group
void Model::add_agent_component_to_group(shared_ptr<AgentComponent> component,
                                         shared_ptr<AgentComponent> group)
{
  // Check if the component is already in another group.
  if (is_agent_component_in_group(component)) {
    throw Error("Agent component is already in a group!");
  }
  
  // Will throw if add is not valid.
  group->add_component(component);
  agent_components.erase(component->get_name());
}

// remove an individual agent, should only be called internally upon death.
// assumes that named agent exists
void Model::remove_agent(const string& name)
{
  // If we didn't erase it from our map, remove it from all components.
  if (!agent_components.erase(name)) {
    for (auto& component : agent_components) {
      component.second->remove_component_if_present(name);
    }
  }
  // Take him out of our sim objects as well.
  objects.erase(name);
}

// remove an agent component, don't touch sim objects
void Model::remove_agent_component(const string& name)
{
  // Hold on to the component so we can disband it after removing it.
  auto component = get_agent_comp_ptr(name);
  
  // If the component is not in a group, don't disband.
  if (is_agent_component_in_group(component)) {
    throw Error("Cannot disband object inside group!");
  }
  
  component->disband();
  
  // Actually erase the group, we know it has been disbanded.
  agent_components.erase(name);
}

// removes an existing agent component, does nothing with sim_objects
void Model::remove_agent_component_from_group(shared_ptr<AgentComponent> component,
                                              shared_ptr<AgentComponent> group)
{
  group->remove_component(component->get_name());
  
  // Add the component back to our collection of components.
  agent_components[component->get_name()] = component;
}

// will throw Error("Agent/Component not found!") if no agent component of that name
shared_ptr<AgentComponent> Model::get_agent_comp_ptr(const string& name) const
{
  for (auto& component : agent_components) {
    auto found_component = component.second->get_component(name);
    if (found_component) {
      return found_component;
    }
  }
  throw Error("Agent or Group not found!");
}

// returns the closest agent to the provided object (excluding 'object' itself)
shared_ptr<AgentComponent> Model::closest_agent_in_range_not_in_group(shared_ptr<Sim_object> object,
                                                                      double range) const
{
  shared_ptr<AgentIndividual> closest_individual;
  for (auto& component : agent_components) {
    auto individual = component.second->get_nearest_in_range(object, range);
    
    // ignore if out of range or not in the same group
    if (!individual || are_in_same_group(object->get_name(), individual->get_name())) {
      continue;
    }
    
    if (!closest_individual ||
        cartesian_distance(object->get_location(), individual->get_location()) <
        cartesian_distance(object->get_location(), closest_individual->get_location())) {
      closest_individual = individual;
    }
  }
  
  return closest_individual;
}

// returns all agents within a certain range (excluding 'center' sim_object)
shared_ptr<AgentComponent> Model::find_agents_in_range(shared_ptr<Sim_object> center,
                                                       double range) const
{
  auto agents_in_range = make_shared<AgentGroup>("");
  for (auto& component : agent_components) {
    auto comp = component.second->get_all_in_range(center, range);
    if (comp) {
      agents_in_range->add_component(comp);
    }
  }
  return agents_in_range;
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

// protected helpers //

// dont broadcast state
void Model::insert_structure(shared_ptr<Structure>s)
{
  auto spair = make_pair(s->get_name(), s);
  objects.insert(spair);
  structures.insert(spair);
}

void Model::insert_new_agent(shared_ptr<AgentIndividual> new_agent)
{
  add_new_agent_component(new_agent);
  objects[new_agent->get_name()] = new_agent;
}

