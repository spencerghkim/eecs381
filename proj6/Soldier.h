#ifndef SOLDIER_H_
#define SOLDIER_H_

#include "Warrior.h"

#include <memory>

struct Point;

class Soldier : public Warrior {
public:
	
	Soldier(const std::string& name_, Point location_);
  
	// Overrides Agent's take_hit to counterattack when attacked.
	void take_hit(int attack_strength, std::shared_ptr<Agent> attacker_ptr) override;
	
	// output information specific to Soldier
	void describe() const override;
  
  // return the battle cry for a Soldier
  std::string get_battle_cry() override;
};

#endif
