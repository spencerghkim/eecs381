#ifndef ARCHER_H_
#define ARCHER_H_

#include "Warrior.h"

#include <memory>
#include <string>

struct Point;

class Archer : public Warrior {
public:
  
  Archer(const std::string& name_, Point location);
  
  // update implements Archer's behavior
  void update() override;
  
  // Overrides Agent's take_hit to run away to the nearest Structure when attacked.
  void take_hit(int attack_strength, std::shared_ptr<Agent> attacker) override;
  
  // output information specific to Archer
  void describe() const override;
  
  std::string get_battle_cry() override;
};

#endif
