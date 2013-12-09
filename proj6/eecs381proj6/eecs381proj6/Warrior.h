#ifndef WARRIOR_H_
#define WARRIOR_H_

/*
 
 A Warior is an AgentComponent that has attack and defense behaviors. It can be commanded
 to start attacking another AgentComponent and to move to a certain location.

*/

#include "AgentIndividual.h"

#include <memory>

struct Point;

class Warrior : public AgentIndividual {
public:
  
  Warrior(const std::string& name_, Point location_, int strength_, double range_);
  
  // Abstract class.
  virtual ~Warrior() = 0;
  
  // update implements a Warrior's general behavior
  void update() override;
  
  // Make this Warrior start attacking the target AgentComponent.
	// Throws an exception if the target is the same as this AgentComponent,
	// is out of range, or is not alive.
  void start_attacking(std::shared_ptr<AgentComponent> target_ptr) override;
  
  // Overrides AgentComponent's stop to print a message
  void stop() override;
  
  // Describe the state of the object.
  void describe() const override;
  
  // ask Model to broadcast our current state to all Views
  void broadcast_current_state() override;
  
protected:
  
  bool is_attacking() const
    { return attacking; }
  
  // Attack the given agent, regardless of if its status (in range, alive, etc.)
  void attack(std::shared_ptr<AgentComponent> target_ptr);
  // attack is over, clear target and set state
  void clear_attack();
  
private:
  
  int attack_strength;
  double attack_range;
  bool attacking;
  
  std::weak_ptr<AgentComponent> target;
  
  // Ask subclasses for their battle cry.
  virtual std::string get_battle_cry() = 0;
};

#endif
