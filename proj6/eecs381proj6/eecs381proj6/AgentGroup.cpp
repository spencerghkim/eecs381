#include "AgentGroup.h"

#include "AgentIndividual.h"
#include "Geometry.h"
#include "Utility.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std::placeholders; using std::bind;
using std::vector; using std::map; using std::string;
using std::cout; using std::endl;
using std::function;
using std::shared_ptr; using std::make_shared;

AgentGroup::AgentGroup(const std::string &name_) :
    group_name{name_} {}

// does any one in the group have this prefix?
bool AgentGroup::has_prefix(const std::string& prefix)
{
  for (auto& component: group_components) {
    if (component.second->has_prefix(prefix)) {
      return true;
    }
  }
  return false;
}

// iterate over the contained components and handle errors
void AgentGroup::iterate_and_catch(function<void(AgentComponent*)> func) {
  for (auto& component : group_components) {
    try {
      func(component.second.get());
    } catch (Error &e) {
      //TODO: handle clear line?
      cout << e.msg << endl;
    }
  }
}

// get the nearest agent individual in the group
shared_ptr<AgentIndividual> AgentGroup::get_nearest_in_range(shared_ptr<const Sim_object> origin,
                                                             double range) {
  shared_ptr<AgentIndividual> best;
  for (auto& component : group_components) {
    auto cur = component.second->get_nearest_in_range(origin, range);
    
    // Make sure cur is valid and different than our origin argument.
    if (!cur || origin->get_name() == cur->get_name()) {
      continue;
    }
    
    // If best is not set, set it to cur, which is in range.
    if (!best) {
      best = cur;
    } else {
      // Otherwise, compare distances to see who is closer.     
      if (cartesian_distance(origin->get_location(), cur->get_location()) <
          cartesian_distance(origin->get_location(), best->get_location())) {
        best = cur;
      }
    }
  }
  return best;
}

// get the nearest agents in range, excluding the 'origin' sim_object
shared_ptr<AgentComponent> AgentGroup::get_all_in_range(shared_ptr<const Sim_object> origin,
                                                        double range)
{
  auto agents_in_range = make_shared<AgentGroup>("temp");
  for (auto& component : group_components) {
    auto component_in_range = component.second->get_all_in_range(origin, range);
    if (component_in_range && component_in_range->get_name() != origin->get_name()) {
      agents_in_range->add_component(component_in_range);
    }
  }
  return agents_in_range;
}

// tell this AgentComponent to start moving to location destination_
void AgentGroup::move_to(Point destination_)
{
  iterate_and_catch(bind(&AgentComponent::move_to, _1, destination_));
}

// tell this AgentComponent to stop its activity
void AgentGroup::stop()
{
  iterate_and_catch(bind(&AgentComponent::stop,_1));
}

void AgentGroup::start_working(std::shared_ptr<Structure> s1, std::shared_ptr<Structure> s2)
{
  iterate_and_catch(bind(&AgentComponent::start_working, _1, s1, s2));
}

void AgentGroup::start_attacking(std::shared_ptr<AgentComponent> target)
{
  iterate_and_catch(bind(&AgentComponent::start_attacking, _1, target));
}

// Call accept blessing for everyone.
void AgentGroup::accept_blessing(int blessing_strength, std::shared_ptr<AgentIndividual> blesser_ptr)
{
  iterate_and_catch(bind(&AgentComponent::accept_blessing, _1, blessing_strength, blesser_ptr));
}

// Will throw if trying to add self or parent.
void AgentGroup::add_component(std::shared_ptr<AgentComponent> component)
{
  // Check for self assignment.
  if (component->get_name() == get_name()) {
    throw Error("Can't add component to self!");
  }
  
  // Check for cycles.
  if (component->get_component(get_name())) {
    throw Error("Can't add parent component to self!");
  }
  
  group_components[component->get_name()] = component;
}

shared_ptr<AgentComponent> AgentGroup::get_component(const string& name_)
{
  // Check to see if we're asking for ourself.
  if (get_name() == name_) {
    return shared_from_this();
  }
  
  // Recursively ask components to search for name_.
  for (auto& component : group_components) {
    auto found_component = component.second->get_component(name_);
    if (found_component) {
      return found_component;
    }
  }
  // return empty pointer
  return nullptr;
}

void AgentGroup::remove_component(const string& name_)
{
  // Check to see if we're trying to remove ourself.
  if (name_ == get_name()) {
    throw Error("Can't remove group from self!");
  }
  
  auto comp = group_components.find(name_);
  // Check to make sure that we directly own that component.
  if (comp == group_components.end()) {
    throw Error("Group does not directly contain that component!");
  }
  
  // if we're removing a group, disband it
  comp->second->disband_from_group();
  group_components.erase(name_);
}

void AgentGroup::remove_component_if_present(const std::string& name_)
{
  if (group_components.erase(name_)) {
    // We found it and erased it. Get out.
    return;
  }
  
  // Otherwise, remove components iteratively.
  for (auto& component : group_components) {
    component.second->remove_component_if_present(name_);
  }
}

// clear out group and put agents back in model root
void AgentGroup::disband() {
  disband_from_group();
}

// disband the component to the root of the model
void AgentGroup::disband_from_group() {
  for (auto& component : group_components) {
    component.second->disband_from_group();
  }
}

