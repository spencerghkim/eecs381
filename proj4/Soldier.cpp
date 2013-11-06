#include "Soldier.h"

#include "Agent.h"
#include "Geometry.h"
#include "Moving_object.h"
#include "Sim_object.h"
#include "Utility.h"

#include <iostream>

#define ATTACK_STRENGTH 2
#define ATTACK_RANGE 2.0

using std::cout; using std::endl;

// *** define as specified
Soldier::Soldier(const std::string& name_, Point location_)
    : Agent(name_, location_),
      attack_strength{ATTACK_STRENGTH},
      attack_range{ATTACK_RANGE},
      target{nullptr},
      state{NOT_ATTACKING}
{
  cout << "Soldier " << name_ << " constructed" << endl;
}
  
Soldier::~Soldier()
{
  cout << "Soldier " << get_name() << " destructed" << endl;
}

// update implements Soldier behavior
void Soldier::update()
{
  Agent::update();
  if (!is_alive() || state == NOT_ATTACKING) {
    return;
  }
  
  // Check if the target is still alive.
  if (!target->is_alive()) {
    cout << get_name() << ": Target is dead" << endl;
    state = NOT_ATTACKING;
    target = nullptr;
    return;
  }
  
  // Check if the target is still in range.
  if (cartesian_distance(get_location(), target->get_location()) > attack_range) {
    cout << get_name() << ": Target is now out of range" << endl;
    state = NOT_ATTACKING;
    return;
  }
  
  // Attack!
  cout << get_name() << ": Clang!" << endl;
  target->take_hit(attack_strength, this);
      
  // Did we just kill it?
  if (!target->is_alive()) {
    cout << get_name() << ": I triumph!" << endl;
    state = NOT_ATTACKING;
    target = nullptr;
  }
}

// Make this Soldier start attacking the target Agent.
// Throws an exception if the target is the same as this Agent,
// is out of range, or is not alive.
void Soldier::start_attacking(Agent* target_ptr)
{
  if (target_ptr == this) {
    throw Error{ get_name() + ": I cannot attack myself!" };
  }
  
  if (!target_ptr->is_alive()) {
    throw Error{ get_name() + ": Target is not alive!" };
  }
  
  if (cartesian_distance(get_location(), target_ptr->get_location()) > attack_range) {
    throw Error{ get_name() + ": Target is out of range!" };
  }
  
  cout << get_name() << ": I'm attacking!" << endl;
  target = target_ptr;
  state = ATTACKING;
}

// Overrides Agent's take_hit to counterattack when attacked.
void Soldier::take_hit(int attack_strength, Agent* attacker_ptr)
{
  Agent::lose_health(attack_strength);
  
  if (state == ATTACKING && !is_alive()) {
    state = NOT_ATTACKING;
    target = nullptr;
  }
  
  if (state == NOT_ATTACKING && is_alive() && attacker_ptr->is_alive()) {
    // Hit back!
    state = ATTACKING;
    target = attacker_ptr;
    cout << get_name() << ": I'm attacking!" << endl;
  }
}

// Overrides Agent's stop to print a message
void Soldier::stop()
{
  cout << get_name() << ": Don't bother me" << endl;
}

// output information about the current state
void Soldier::describe() const
{
  cout << "Soldier ";
  Agent::describe();
  
  if (state == ATTACKING) {
    cout << "   Attacking " << target->get_name() << endl;
  } else {
    cout << "   Not attacking" << endl;
  }
}