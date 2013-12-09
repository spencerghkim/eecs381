#include "Warrior.h"

#include "AgentComponent.h"
#include "Model.h"
#include "Utility.h"

#include <iostream>
#include <cassert>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr;

Warrior::Warrior(const string& name_, Point location_, int strength_, double range_) :
  AgentIndividual{name_, location_},
  attack_strength{strength_},
  attack_range{range_},
  attacking{false} {}

// Explicit default destructor for Warrior.
Warrior::~Warrior() {}

// update implements Warrior behavior
void Warrior::update()
{
  AgentIndividual::update();
  
  // Do nothing if we aren't attacking.
  if (!is_attacking()) {
    return;
  }
  
  // Check if our target no longer exists or is dead.
  shared_ptr<AgentComponent> target_ptr = target.lock();
  if (!target_ptr) {
    cout << get_name() << ": Target is dead" << endl;
    clear_attack();
    return;
  }
  
  auto closest_indv = target_ptr->get_closest(get_location());
  
  // Check if the target is still in range.
  if (cartesian_distance(get_location(), closest_indv->get_location()) > attack_range) {
    cout << get_name() << ": Target is now out of range" << endl;
    clear_attack();
    return;
  }
  
  // Attack!
  cout << get_name() << ": " << get_battle_cry() << endl;
  closest_indv->take_hit(attack_strength, shared_from_this());
  
  // Did we just kill it?
  if (!closest_indv->is_alive()) {
    cout << get_name() << ": I triumph!" << endl;
    auto old_target = target_ptr;
    clear_attack();
    start_attacking(old_target);
  }
}

// Make this Soldier start attacking the target AgentComponent.
// Throws an exception if the target is the same as this AgentComponent,
// is out of range, or is not alive.
void Warrior::start_attacking(shared_ptr<AgentComponent> target_ptr)
{
  if (target_ptr == shared_from_this()) {
    
    //TODO: add check for attacking own group
    throw Error( get_name() + ": I cannot attack myself!" );
  }
  
  auto closest_indv = target_ptr->get_closest(get_location());
  
  // cant have empty group, must be alive
  assert(closest_indv && closest_indv->is_alive());
  
  if (cartesian_distance(get_location(), closest_indv->get_location()) > attack_range) {
    throw Error( get_name() + ": Target is out of range!" );
  }
  
  attack(target_ptr);
  Model::get().notify_attack(get_name(), closest_indv->get_name());
}

// Attack the given agent, regardless of if its status (in range, alive, etc.)
void Warrior::attack(shared_ptr<AgentComponent> target_ptr)
{
  cout << get_name() << ": I'm attacking!" << endl;
  target = target_ptr;
  attacking = true;
}

// attack is over, clear target and set state
void Warrior::clear_attack()
{
  Model::get().notify_end_attack(get_name());
  attacking = false;
  target.reset();
}

void Warrior::stop()
{
  cout << get_name() << ": Don't bother me" << endl;
}

// output information about the current state
void Warrior::describe() const
{
  AgentIndividual::describe();
  
  shared_ptr<AgentComponent> target_ptr = target.lock();
  if (is_attacking()) {
    if (target_ptr) {
      auto closest = target_ptr->get_closest(get_location());
      //TODO: fix this too!
      cout << "   Attacking " << closest->get_name() << endl;
    } else {
      cout << "   Attacking dead target" << endl;
    }
  } else {
    cout << "   Not attacking" << endl;
  }
}

void Warrior::broadcast_current_state()
{
  AgentIndividual::broadcast_current_state();
  if (is_attacking()) {
    auto target_ptr = target.lock();
    assert(target_ptr);
    
    auto closest = target_ptr->get_closest(get_location());
    if(closest) { //TODO: is this always right?
      Model::get().notify_attack(get_name(), closest->get_name());
    }
  }
}
