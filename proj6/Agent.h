#ifndef AGENT_H_
#define AGENT_H_

/*
 Agents are a kind of Sim_object, and privately inherit from Moving_object.
 Agents can be commanded to move to a destination. Agents have a health, which
 is decreased when they take a hit. If the Agent's health > 0, it is alive.
 If its heath <= 0, it starts dying, then on subsequent updates,
 it becomes dead, and finally disappearing.
 */

#include "Moving_object.h"
#include "Sim_object.h"

#include <memory>

class Structure;
struct Point;

class Agent : public Sim_object, public Moving_object, public std::enable_shared_from_this<Agent> {
public:
	// create with initial health is 5, speed is 5, state is Alive
	Agent(const std::string& in_name, Point in_location);
	
  // class is abstract, make destructor pure virtual
  virtual ~Agent() = 0;
  
	// return true if this agent is Alive
	bool is_alive() const { return state == ALIVE; }
	
	// return this Agent's location
	Point get_location() const override;
  
	// return true if this Agent is in motion
	bool is_moving() const;
	
	// tell this Agent to start moving to location destination_
	virtual void move_to(Point destination_);
  
	// tell this Agent to stop its activity
	virtual void stop();
  
	// Tell this Agent to accept a hit from an attack of a specified strength
	// The attacking Agent identifies itself with its this pointer.
	// A derived class can override this function.
	// The function lose_health is called to handle the effect of the attack.
	virtual void take_hit(int attack_strength, std::shared_ptr<Agent> attacker_ptr);
  
  // Tell this Agent to accept a blessing from an agent specified by blesser_ptr.
  // A derived class may override this function, default behavior is nothing.
  // The function gain_health may be called to handle the effect of the blessing.
  virtual void accept_blessing(int blessing_strength, std::shared_ptr<Agent> blesser_ptr) {}
	
	// update the moving state and Agent state of this object.
	void update() override;
	
	// output information about the current state
	void describe() const override;
	
	// ask Model to broadcast our current state to all Views
	void broadcast_current_state() override;
  
	/* Fat Interface for derived classes */
	// Throws exception that an Agent cannot work.
	virtual void start_working(std::shared_ptr<Structure>, std::shared_ptr<Structure>);
  
	// Throws exception that an Agent cannot attack.
	virtual void start_attacking(std::shared_ptr<Agent>);
  
protected:
	// calculate loss of health due to hit.
	// if health decreases to zero or negative, Agent state becomes Dying, and any movement is stopped.
	void lose_health(int attack_strength);
  
  // calculate gain of health due to blessing. if health is already at full, or this agent is dead,
  // this method has no effect
  void gain_health(int blessing_strength);
  
private:
  typedef enum {
    ALIVE,
    DEAD
  } Agent_state_e;
  
  int health;
  Agent_state_e state;
};


#endif
