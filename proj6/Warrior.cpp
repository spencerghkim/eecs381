#include "Warrior.h"

#include "Agent.h"
#include "Model.h"
#include "Utility.h"

#include <iostream>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr;

Warrior::Warrior(const string& name_, Point location_, int strength_, double range_) :
  Agent(name_, location_),
  attack_strength{strength_},
  attack_range{range_},
  state{NOT_ATTACKING} {}

// Explicit default destructor for Warrior.
Warrior::~Warrior() {}

// update implements Warrior behavior
void Warrior::update()
{
  Agent::update();
  
  // Do nothing if we aren't attacking.
  if (state == NOT_ATTACKING) {
    return;
  }
  
  // Check if our target no longer exists or is dead.
  shared_ptr<Agent> target_ptr = target.lock();
  if (!target_ptr || !target_ptr->is_alive()) {
    cout << get_name() << ": Target is dead" << endl;
    state = NOT_ATTACKING;
    target.reset();
    return;
  }
  
  // Check if the target is still in range.
  if (cartesian_distance(get_location(), target_ptr->get_location()) > attack_range) {
    cout << get_name() << ": Target is now out of range" << endl;
    state = NOT_ATTACKING;
    return;
  }
  
  // Attack!
  cout << get_name() << ": " << get_battle_cry() << endl;
  target_ptr->take_hit(attack_strength, shared_from_this());
  
  // Did we just kill it?
  if (!target_ptr->is_alive()) {
    cout << get_name() << ": I triumph!" << endl;
    state = NOT_ATTACKING;
    target.reset();
  }
}

// Make this Soldier start attacking the target Agent.
// Throws an exception if the target is the same as this Agent,
// is out of range, or is not alive.
void Warrior::start_attacking(shared_ptr<Agent> target_ptr)
{
  if (target_ptr == shared_from_this()) {
    throw Error{ get_name() + ": I cannot attack myself!" };
  }
  
  if (!target_ptr->is_alive()) {
    throw Error{ get_name() + ": Target is not alive!" };
  }
  
  if (cartesian_distance(get_location(), target_ptr->get_location()) > attack_range) {
    throw Error{ get_name() + ": Target is out of range!" };
  }
  
  attack(target_ptr);
}

void Warrior::attack(shared_ptr<Agent> target_ptr)
{
  cout << get_name() << ": I'm attacking!" << endl;
  target = target_ptr;
  state = ATTACKING;
}

void Warrior::stop()
{
  cout << get_name() << ": Don't bother me" << endl;
}

// output information about the current state
void Warrior::describe() const
{
  Agent::describe();
  
  shared_ptr<Agent> target_ptr = target.lock();
  if (state == ATTACKING) {
    if (target_ptr) {
      cout << "   Attacking " << target_ptr->get_name() << endl;
    } else {
      cout << "   Attacking dead target" << endl;
    }
  } else {
    cout << "   Not attacking" << endl;
  }
}
