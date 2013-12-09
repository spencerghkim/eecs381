
#include "Magician.h"

#include "AgentComponent.h"
#include "Model.h"
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

const int MAGICIAN_ATTACK_STRENGTH = 4;
const double MAGICIAN_ATTACK_RANGE = numeric_limits<double>::max();
const int MAGICIAN_BLESSING_STRENGTH = 1;
const double MAGICIAN_BLESSING_RANGE = 10.0;

Magician::Magician(const string& name_, Point location_) :
  Warrior{name_, location_, MAGICIAN_ATTACK_STRENGTH, MAGICIAN_ATTACK_RANGE},
  blessing_strength{MAGICIAN_BLESSING_STRENGTH},
  blessing_range{MAGICIAN_BLESSING_RANGE} {}

void Magician::update()
{
  Warrior::update();
  
  // If we aren't attacking, bless the peaceful people.
  if (!is_attacking()) {
    auto agents_in_range = Model::get().find_agents_in_range(shared_from_this(), blessing_range);
    //TODO: fix this shitshow
    /*for_each(agents_in_range.begin(),
             agents_in_range.end(),
             bind(&AgentIndividual::accept_blessing, _1, blessing_strength, shared_from_this()));*/
  }
}

// Magicians are wily, but poorly armored. A hit only has a 50% chance of landing, but
// if it does, it kills the Magician immediately, regardless of attack_strength.
void Magician::take_hit(int attack_strength, shared_ptr<AgentIndividual> attacker_ptr)
{
  // "random" simulation
  // ideally we'd use a *real* random number here
  static int rand = 0;
  
  // change to rand() % 2 and remove the static int
  int hit_landed = rand++ % 2;
  
  if (hit_landed == 1) {
    // The hit landed.
    AgentIndividual::take_hit(numeric_limits<int>::max(), attacker_ptr);
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
  return "Zap!";
}
