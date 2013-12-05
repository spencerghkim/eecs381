#ifndef MAGICIAN_H_
#define MAGICIAN_H_

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
