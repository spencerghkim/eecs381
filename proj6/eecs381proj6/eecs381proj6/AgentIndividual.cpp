#include "AgentIndividual.h"

#include "Geometry.h"
#include "Model.h"
#include "Utility.h"

#include <cassert>
#include <iostream>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr;

const double DEFAULT_SPEED = 5.0;
const int INITIAL_HEALTH = 5;

AgentIndividual::AgentIndividual(const string& in_name, Point in_location) :
  AgentComponent{in_name},
  Moving_object{in_location, DEFAULT_SPEED},
  health{INITIAL_HEALTH},
  alive{true} {}

// Explicit default destructor.
AgentIndividual::~AgentIndividual() {}

std::shared_ptr<AgentIndividual> AgentIndividual::get_closest(Point origin)
{
  return shared_from_this();
}

void AgentIndividual::get_closest_h(Point origin, std::shared_ptr<AgentIndividual> &best) {
  auto best_dist = cartesian_distance(best->get_location(), origin);
  auto cur_dist = cartesian_distance(get_location(), origin);
  if (cur_dist < best_dist) {
    best = shared_from_this();
  }
}

// is this individual in range of annother point
bool AgentIndividual::in_range(Point point, double range) {
  return cartesian_distance(point, get_location()) <= range;
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

// calculate loss of health due to hit.
// if health decreases to zero or negative, AgentIndividual state becomes Dying, and any movement is stopped.
void AgentIndividual::lose_health(int attack_strength)
{
  health -= attack_strength;
  if (health <= 0) {
    alive = false;
    Model::get().notify_gone(get_name());
    cout << get_name() << ": Arrggh!" << endl;
    Model::get().remove_agent(shared_from_this());
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

