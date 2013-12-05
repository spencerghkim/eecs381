#include "Warriors.h"

#include "Agent.h"
#include "Geometry.h"
#include "Model.h"
#include "Moving_object.h"
#include "Sim_object.h"
#include "Structure.h"
#include "Utility.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>

using std::vector;
using std::string;
using std::cout; using std::endl;
using std::shared_ptr;
using std::for_each;
using std::bind;
using std::numeric_limits;
using namespace std::placeholders;

const int ARCHER_ATTACK_STRENGTH = 1;
const double ARCHER_ATTACK_RANGE = 6.0;
const int MAGICIAN_ATTACK_STRENGTH = 4;
const double MAGICIAN_ATTACK_RANGE = numeric_limits<double>::max();
const int MAGICIAN_BLESSING_STRENGTH = 1;
const double MAGICIAN_BLESSING_RANGE = 10.0;
const int SOLDIER_ATTACK_STRENGTH = 2;
const double SOLDIER_ATTACK_RANGE = 2.0;

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

Archer::Archer(const string& name_, Point location_)
: Warrior(name_, location_, ARCHER_ATTACK_STRENGTH, ARCHER_ATTACK_RANGE) {}

void Archer::update()
{
  // Warrior::update will attack our target if it's alive, in range, etc..
  Warrior::update();
  
  // If we aren't attacking, look for someone to shoot!
  if (!is_attacking()) {
    auto closest_agent = Model::get().closest_agent(shared_from_this());
    
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
    auto closest_structure = Model::get().closest_structure(shared_from_this());
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

Magician::Magician(const string& name_, Point location_) :
  Warrior(name_, location_, MAGICIAN_ATTACK_STRENGTH, MAGICIAN_ATTACK_RANGE),
  blessing_strength(MAGICIAN_BLESSING_STRENGTH),
  blessing_range(MAGICIAN_BLESSING_RANGE) {}

void Magician::update()
{
  Warrior::update();
  
  // If we aren't attacking, bless the peaceful people.
  if (!is_attacking()) {
    vector<shared_ptr<Agent>> agents_in_range =
    Model::get().find_agents_in_range(shared_from_this(), blessing_range);
    for_each(agents_in_range.begin(),
             agents_in_range.end(),
             bind(&Agent::accept_blessing, _1, blessing_strength, shared_from_this()));
  }
}

// Magicians are wily, but poorly armored. A hit only has a 50% chance of landing, but
// if it does, it kills the Magician immediately, regardless of attack_strength.
void Magician::take_hit(int attack_strength, shared_ptr<Agent> attacker_ptr)
{
  int hit_landed = rand() % 2;
  if (hit_landed == 1) {
    // The hit landed.
    Agent::take_hit(numeric_limits<int>::max(), attacker_ptr);
  } else {
    // The hit missed.
    cout << get_name() << ": Aha! You missed!" << endl;
  }
}

void Magician::describe() const
{
  cout << "Magician ";
  Warrior::describe();
}

string Magician::get_battle_cry()
{
  return "Cast!";
}

Soldier::Soldier(const string& name_, Point location_)
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


