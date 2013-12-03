#include "Warriors.h"

#include "Agent.h"
#include "Geometry.h"
#include "Model.h"
#include "Moving_object.h"
#include "Sim_object.h"
#include "Structure.h"
#include "Utility.h"

#include <iostream>

#define SOLDIER_ATTACK_STRENGTH 2
#define SOLDIER_ATTACK_RANGE 2.0
#define ARCHER_ATTACK_STRENGTH 1
#define ARCHER_ATTACK_RANGE 6.0

using std::string;
using std::cout; using std::endl;
using std::shared_ptr;

Warrior::Warrior(const string& name_, Point location_, int strength_, double range_)
    : Agent(name_, location_),
      attack_strength{strength_},
      attack_range{range_},
      state{NOT_ATTACKING} {}

// Explicit default destructor for Warrior.
Warrior::~Warrior() {}

// update implements Soldier behavior
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

Soldier::Soldier(const std::string& name_, Point location_)
    : Warrior(name_, location_, SOLDIER_ATTACK_STRENGTH, SOLDIER_ATTACK_RANGE) {}

// Overrides Agent's take_hit to counterattack when attacked.
void Soldier::take_hit(int attack_strength, shared_ptr<Agent> attacker_ptr)
{
  Agent::take_hit(attack_strength, attacker_ptr);
  
  if (is_alive() && !is_attacking() && attacker_ptr->is_alive()) {
    // Hit back!
    attack(attacker_ptr);
  }
}

void Soldier::describe() const
{
  cout << "Soldier ";
  Warrior::describe();
}

string Soldier::get_battle_cry()
{
  return "Clang!";
}

Archer::Archer(const std::string& name_, Point location_)
    : Warrior(name_, location_, ARCHER_ATTACK_STRENGTH, ARCHER_ATTACK_RANGE) {}

void Archer::update()
{
  // Warrior::update will attack our target if it's alive, in range, etc..
  Warrior::update();
  
  // If we aren't attacking, look for someone to shoot!
  if (!is_attacking()) {
    auto closest_agent = Model::getInstance().find_closest_agent(shared_from_this());
    
    // Validate closest_agent, then make sure closest_agent is within range.
    if (!closest_agent ||
        cartesian_distance(closest_agent->get_location(), get_location()) > ARCHER_ATTACK_RANGE) {
      return;
    }
    start_attacking(closest_agent);
  }
}

void Archer::take_hit(int attack_strength, shared_ptr<Agent> attacker_ptr)
{
  Agent::take_hit(attack_strength, attacker_ptr);
  
  if (is_alive() && attacker_ptr->is_alive()) {
    // Run away!
    auto closest_structure = Model::getInstance().find_closest_structure(shared_from_this());
    if (!closest_structure) {
      return;
    }
    cout << get_name() << ": I'm going to run away to " << closest_structure->get_name() << endl;
    move_to(closest_structure->get_location());
  }
}

void Archer::describe() const
{
  cout << "Archer ";
  Warrior::describe();
}

string Archer::get_battle_cry()
{
  return "Twang!";
}

