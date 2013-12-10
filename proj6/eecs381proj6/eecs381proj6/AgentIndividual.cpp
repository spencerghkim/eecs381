#include "AgentIndividual.h"

#include "Geometry.h"
#include "Model.h"
#include "Utility.h"

#include <cassert>
#include <iostream>
#include <vector>

using std::string; using std::vector;
using std::cout; using std::endl;
using std::shared_ptr;

const double DEFAULT_SPEED = 5.0;
const int INITIAL_HEALTH = 5;

AgentIndividual::AgentIndividual(const string& in_name, Point in_location) :
  Sim_object{in_name},
  Moving_object{in_location, DEFAULT_SPEED},
  health{INITIAL_HEALTH},
  alive{true} {}

// Explicit default destructor.
AgentIndividual::~AgentIndividual() {}

bool AgentIndividual::has_prefix(const std::string &prefix)
{
  return get_name().substr(0, prefix.size()) == prefix;
}

bool in_range(Point p1, Point p2, double range)
{
  if (cartesian_distance(p1, p2) > range) {
    return false;
  }
  return true;
}

// get shared_from_this
shared_ptr<AgentIndividual> AgentIndividual::get_nearest_in_range(shared_ptr<const Sim_object> origin,
                                                                  double range)
{
  if (in_range(origin->get_location(), get_location(), range)) {
    return {shared_from_this()};
  }
  return {};
}

// return a vector of only this object
shared_ptr<AgentComponent> AgentIndividual::get_all_in_range(shared_ptr<const Sim_object> origin,
                                                             double range)
{
  auto dist = cartesian_distance(origin->get_location(), get_location());
  if (dist <= range) {
    return {shared_from_this()};
  }
  return {};
}

shared_ptr<AgentComponent> AgentIndividual::get_component(const string& name_)
{
  if (get_name() == name_) {
    return shared_from_this();
  }
  return nullptr;
}

// return this AgentIndividual's location
Point AgentIndividual::get_location() const
{
  return get_current_location();
}

// return true if this AgentIndividual is in motion
bool AgentIndividual::is_moving() const
{
  return is_currently_moving();
}

// tell this AgentIndividual to start moving to location destination_
void AgentIndividual::move_to(Point destination_)
{
  start_moving(destination_);
  
  if (is_currently_moving()) {
    cout << get_name() << ": I'm on the way" << endl;
  } else {
    cout << get_name() << ": I'm already there" << endl;
  }
}

// tell this AgentIndividual to stop its activity
void AgentIndividual::stop()
{
  if (is_currently_moving()) {
    stop_moving();
    cout << get_name() << ": I'm stopped" << endl;
  }
}

// Tell this AgentIndividual to accept a hit from an attack of a specified strength
// The attacking AgentIndividual identifies itself with its this pointer.
// A derived class can override this function.
// The function lose_health is called to handle the effect of the attack.
void AgentIndividual::take_hit(int attack_strength, shared_ptr<AgentIndividual>attacker_ptr)
{
  lose_health(attack_strength);
}

// update the moving state and AgentIndividual state of this object.
void AgentIndividual::update()
{
  if (is_alive() && is_currently_moving()) {
    if (update_location()) {
      // We've reached our destination.
      cout << get_name() << ": I'm there!" << endl;
    } else {
      // We're still moving.
      cout << get_name() << ": step..." << endl;
    }
    Model::get().notify_location(get_name(), get_location());
  }
}

// output information about the current state
void AgentIndividual::describe() const
{
  cout << get_name() << " at " << get_location() << endl;
  if (is_alive()) {
    cout << "   Health is " << health << endl;
    if (is_currently_moving()) {
      cout << "   Moving at speed " << get_current_speed() << " to " << get_current_destination() << endl;
    } else {
      cout << "   Stopped" << endl;
    }
  } else {
    cout << "   Is dead" << endl;
  }
}

// ask Model to broadcast our current state to all Views
void AgentIndividual::broadcast_current_state()
{
  Model::get().notify_location(get_name(), get_location());
  Model::get().notify_health(get_name(), health);
}

/* Fat Interface for derived classes */
// Throws exception that an AgentIndividual cannot work.
void AgentIndividual::start_working(shared_ptr<Structure>, shared_ptr<Structure>)
{
  throw Error( get_name() + ": Sorry, I can't work!" );
}

// Throws exception that an AgentIndividual cannot attack.
void AgentIndividual::start_attacking(shared_ptr<AgentComponent>)
{
  throw Error( get_name() + ": Sorry, I can't attack!" );
}

// adds individual back to model's base group
void AgentIndividual::disband_from_group()
{
  Model::get().add_existing_agent_component(shared_from_this());
}

// calculate loss of health due to hit.
// if health decreases to zero or negative, AgentIndividual state becomes Dying, and any movement is stopped.
void AgentIndividual::lose_health(int attack_strength)
{
  health -= attack_strength;
  if (health <= 0) {
    alive = false;
    Model::get().notify_gone(get_name());
    cout << get_name() << ": Arrggh!" << endl;
    Model::get().remove_agent(get_name());
  } else {
    Model::get().notify_health(get_name(), health);
    cout << get_name() << ": Ouch!" << endl;
  }
}

// calculate gain of health due to blessing
void AgentIndividual::gain_health(int blessing_strength)
{
  if (health == INITIAL_HEALTH) {
    return;
  } else {
    cout << get_name() << ": I am healed!" << endl;
    health += blessing_strength;
    
    // If we've gone over our initial health, reset it.
    if (health > INITIAL_HEALTH) {
      health = INITIAL_HEALTH;
    }
    Model::get().notify_health(get_name(), health);
  }
}


