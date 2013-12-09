
#include "Soldier.h"

#include "Utility.h"

#include <iostream>
#include <memory>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr;

const int SOLDIER_ATTACK_STRENGTH = 2;
const double SOLDIER_ATTACK_RANGE = 2.0;

Soldier::Soldier(const string& name_, Point location_) :
  Warrior{name_, location_, SOLDIER_ATTACK_STRENGTH, SOLDIER_ATTACK_RANGE} {}

// Overrides AgentComponent's take_hit to counterattack when attacked.
void Soldier::take_hit(int attack_strength, shared_ptr<AgentIndividual> attacker_ptr)
{
  AgentIndividual::take_hit(attack_strength, attacker_ptr);
  
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