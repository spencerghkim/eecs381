
#include "Archer.h"

#include "Model.h"
#include "Structure.h"
#include "Utility.h"

#include <iostream>
#include <vector>

using std::vector;
using std::string;
using std::cout; using std::endl;
using std::shared_ptr;

const int ARCHER_ATTACK_STRENGTH = 1;
const double ARCHER_ATTACK_RANGE = 6.0;

Archer::Archer(const string& name_, Point location_) :
  Warrior{name_, location_, ARCHER_ATTACK_STRENGTH, ARCHER_ATTACK_RANGE} {}

void Archer::update()
{
  // Warrior::update will attack our target if it's alive, in range, etc..
  Warrior::update();
  
  // If we aren't attacking, look for someone to shoot!
  if (!is_attacking()) {
    auto closest_agent = Model::get().closest_agent(shared_from_this());
    
    // Validate closest_agent, then make sure closest_agent is within range.
    /*if (!closest_agent ||
        cartesian_distance(closest_agent->get_location(), get_location()) > ARCHER_ATTACK_RANGE) {
      return;
     }*/
    
    //TODO: fix this use new way
    if (closest_agent) { //TODO: added, may not be needed later
      start_attacking(closest_agent);
    }
  }
}

void Archer::take_hit(int attack_strength, shared_ptr<AgentIndividual> attacker_ptr)
{
  AgentIndividual::take_hit(attack_strength, attacker_ptr);
  
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
