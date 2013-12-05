#ifndef WARRIOR_H_
#define WARRIOR_H_

/*
 A Warior is an Agent that has attack and defense behaviors. It can be commanded
 to start attacking another Agent and will continue the attack as long as
 it is alive and the target is alive and in range.
 */

#include "Agent.h"

#include <memory>

struct Point;

class Warrior : public Agent {
public:
  
  Warrior(const std::string& name_, Point location_, int strength_, double range_);
  
  // Abstract class.
  virtual ~Warrior() = 0;
  
  // update implements a Warrior's general behavior
  void update() override;
  
  // Make this Warrior start attacking the target Agent.
	// Throws an exception if the target is the same as this Agent,
	// is out of range, or is not alive.
  void start_attacking(std::shared_ptr<Agent> target_ptr) override;
  
  // Overrides Agent's stop to print a message
  void stop() override;
  
  // Describe the state of the object.
  void describe() const override;
  
protected:
  
  bool is_attacking()
  { return state == ATTACKING; }
  
  // Attack the given agent, regardless of if its status (in range, alive, etc.)
  void attack(std::shared_ptr<Agent> target_ptr);
  
private:
  typedef enum {
    ATTACKING,
    NOT_ATTACKING
  } Warrior_state_e;
  
  int attack_strength;
  double attack_range;
  Warrior_state_e state;
  std::weak_ptr<Agent> target;
  
  // Ask subclasses for their battle cry.
  virtual std::string get_battle_cry() = 0;
};

#endif
