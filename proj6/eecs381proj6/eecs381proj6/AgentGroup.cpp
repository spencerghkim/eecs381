#include "AgentGroup.h"

#include "AgentIndividual.h"
#include "Geometry.h"
#include "Utility.h"

#include <algorithm>
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
using std::shared_ptr;

AgentGroup::AgentGroup(const std::string &name_) :
  name{name_} {}

// iterate over the contained components and handle errors
void AgentGroup::iterate_and_catch(function<void(AgentComponent*)> func) {
  for (auto& component : group_members) {
    try {
      func(component.second.get());
    } catch (Error &e) {
      //TODO: handle
      cout << e.msg << endl;
    }
  }
}

// get the nearest agent in the group or the single agent
shared_ptr<AgentIndividual> AgentGroup::get_nearest(shared_ptr<const Sim_object> origin) {
  shared_ptr<AgentIndividual> best;
  for (auto& component : group_members) {
    auto cur = component.second->get_nearest(origin);
    auto cur_dist = cartesian_distance(origin->get_location(), cur->get_location());
    auto best_dist = cartesian_distance(origin->get_location(), best->get_location());
    
    // set the closest agent when in range, and not the calling agent
    if (cur_dist < best_dist && origin->get_name() != best->get_name()) {
      best = cur;
    }
  }
  return best;
}


// get the nearest agents in range
vector<shared_ptr<AgentIndividual>> AgentGroup::get_nearest_in_range(shared_ptr<const Sim_object> origin, double range)
{
  vector<shared_ptr<AgentIndividual>> agents;
  for (auto& component : group_members) {
    auto arr = component.second->get_nearest_in_range(origin, range);
    agents.insert(agents.end(), arr.begin(), arr.end());
  }
  return agents;
}

// is anyone in this group in range?
bool AgentGroup::in_range(Point point, double range)
{
  for (auto& component : group_members) {
    if (component.second->in_range(point, range)) {
      return true;
    }
  }
  return false;
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

