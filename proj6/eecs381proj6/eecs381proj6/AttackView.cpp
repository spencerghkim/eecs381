
#include "AttackView.h"

#include "Geometry.h"

#include <iostream>
#include <set>

using std::cout; using std::endl;
using std::string;
using std::set;

// number of dots per unit of distance
const int DISTANCE_MULT = 3;
const string DOT = ". ";
const string ATTACK_SYMBOL_LEFT = " > ";
const string ATTACK_SYMBOL_RIGHT = "< ";

// a new attack has started, keep track of it
void AttackView::update_attack(const std::string& name, const std::string& target)
{
  attackers[name] = target;
}

// an attack has ended, remove it
void AttackView::update_end_attack(const std::string& name)
{
  attackers.erase(name);
}

// an agent involved in an attack has lost health
void AttackView::update_health(const std::string& name, int health)
{
  healths[name] = health;
}

// an agent involved in an attack has moved
void AttackView::update_location(const std::string &name, Point location)
{
  locations[name] = location;
}

// Notify for removal of object
void AttackView::update_remove(const std::string& name)
{
  attackers.erase(name);
  healths.erase(name);
  locations.erase(name);
}

// prints out the view
void AttackView::draw()
{
  if (!attackers.size()) {
    cout << "No Active Duels!" << endl;
    return;
  }
  
  cout << "Current Duels:" << endl;
  set<string> printed;
  
  for (auto &pair : attackers) {
    auto &attacker = pair.first;
    auto &target = pair.second;
    auto a_health = healths[attacker];
    auto t_health = healths[target];
    
    // we already printed the other side of this duel
    if (printed.find(attacker) != printed.end()) continue;
    
    // get the truncated distance between the duel
    int dist = cartesian_distance(locations[attacker], locations[target]) * DISTANCE_MULT;
    
    cout << "(" << a_health << ") " << attacker << ATTACK_SYMBOL_LEFT;
    
    // print dots to represent distance in 2d
    for (int i = 0; i < dist; ++i) {
      cout << DOT;
    }
    
    auto target_itr = attackers.find(target);
    // check if our target is attacking us back
    if (target_itr != attackers.end() && target_itr->second == attacker) {
      // remember the attacking targets we've already printed
      printed.insert(target_itr->first);
      cout << ATTACK_SYMBOL_RIGHT;
    }
    cout << target << " (" << t_health << ")"  << endl;
    
  }
}

// discards the view's contents
void AttackView::clear()
{
  attackers.clear();
  healths.clear();
  locations.clear();
}
