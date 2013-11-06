#ifndef SOLDIER_H_
#define SOLDIER_H_

/*
A Soldier is an Agent that has attack and defense behaviors. It can be commanded
to start attacking another Agent and will continue the attack as long as 
it is alive and the target is alive and in range. If attacked, the Soldier will
start attacking its attacker.
*/

#include "Agent.h"

struct Point;

class Soldier : public Agent {
public:
	
	// *** define as specified
	Soldier(const std::string& name_, Point location_);
	~Soldier();
	
	// update implements Soldier behavior
	void update() override;
	
	// Make this Soldier start attacking the target Agent.
	// Throws an exception if the target is the same as this Agent,
	// is out of range, or is not alive.
	void start_attacking(Agent* target_ptr) override;
	
	// Overrides Agent's take_hit to counterattack when attacked.
	void take_hit(int attack_strength, Agent* attacker_ptr) override;
	
	// Overrides Agent's stop to print a message
	void stop() override;

	// output information about the current state
	void describe() const override;
  
private:
  typedef enum {
    ATTACKING,
    NOT_ATTACKING
  } Soldier_state_e;
  
  int attack_strength;
  double attack_range;
  Agent* target;
  Soldier_state_e state;
};

#endif
