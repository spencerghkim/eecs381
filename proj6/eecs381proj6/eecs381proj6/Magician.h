#ifndef MAGICIAN_H_
#define MAGICIAN_H_

/*
 
 A Magician is a type of Warrior. It has an infinite attack range and
 an attack strength of 4 by default. A Magician can also bless nearby
 peasants within a range of 10. When attacked, a Magicain as a 50/50
 chance of survival.
 
*/

#include "Warrior.h"

#include <memory>

struct Point;

class Magician : public Warrior {
public:
  
  Magician(const std::string& name_, Point location_);
  
  // update implementes Magician's behavior
  void update() override;
  
  // Overrides Agent's take_hit to see if hit actually lands. If so, the Magician is killed
  // regardless of attack_strength.
  void take_hit(int attack_strength, std::shared_ptr<Agent> attacker_ptr) override;
  
  // output information specific to magician
  void describe() const override;
  
  // return the battle cry for a Magician
  std::string get_battle_cry() override;
  
private:
  int blessing_strength;
  double blessing_range;
};

#endif
