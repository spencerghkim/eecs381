#include "Agent.h"

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

Agent::Agent(const string& in_name, Point in_location) :
  Sim_object(in_name),
  Moving_object(in_location, DEFAULT_SPEED),
  health{INITIAL_HEALTH},
  state(ALIVE) {}

// Explicit default destructor.
Agent::~Agent() {}

// return this Agent's location
Point Agent::get_location() const
{
  return get_current_location();
}

// return true if this Agent is in motion
bool Agent::is_moving() const
{
  return is_currently_moving();
}

// tell this Agent to start moving to location destination_
void Agent::move_to(Point destination_)
{
  start_moving(destination_);
  
  if (is_currently_moving()) {
    cout << get_name() << ": I'm on the way" << endl;
  } else {
    cout << get_name() << ": I'm already there" << endl;
  }
}

// tell this Agent to stop its activity
void Agent::stop()
{
  if (is_currently_moving()) {
    stop_moving();
    cout << get_name() << ": I'm stopped" << endl;
  }
}

// Tell this Agent to accept a hit from an attack of a specified strength
// The attacking Agent identifies itself with its this pointer.
// A derived class can override this function.
// The function lose_health is called to handle the effect of the attack.
void Agent::take_hit(int attack_strength, shared_ptr<Agent>attacker_ptr)
{
  lose_health(attack_strength);
}

// update the moving state and Agent state of this object.
void Agent::update()
{
  if (state == ALIVE && is_currently_moving()) {
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
void Agent::describe() const
{
  cout << get_name() << " at " << get_location() << endl;
  if (state == ALIVE) {
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
void Agent::broadcast_current_state()
{
  Model::get().notify_location(get_name(), get_location());
  Model::get().notify_health(get_name(), health);
}

/* Fat Interface for derived classes */
// Throws exception that an Agent cannot work.
void Agent::start_working(shared_ptr<Structure>, shared_ptr<Structure>)
{
  throw Error( get_name() + ": Sorry, I can't work!" );
}

// Throws exception that an Agent cannot attack.
void Agent::start_attacking(shared_ptr<Agent>)
{
  throw Error( get_name() + ": Sorry, I can't attack!" );
}

// calculate loss of health due to hit.
// if health decreases to zero or negative, Agent state becomes Dying, and any movement is stopped.
void Agent::lose_health(int attack_strength)
{
  health -= attack_strength;
  if (health <= 0) {
    state = DEAD;
    Model::get().notify_gone(get_name());
    cout << get_name() << ": Arrggh!" << endl;
    Model::get().remove_agent(shared_from_this());
  } else {
    Model::get().notify_health(get_name(), health);
    cout << get_name() << ": Ouch!" << endl;
  }
}

// calculate gain of health due to blessing
void Agent::gain_health(int blessing_strength)
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

