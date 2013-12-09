#ifndef AGENTINDV_H_
#define AGENTINDV_H_

/*
 AgentIndividuals are a kind of Sim_object, and privately inherit from Moving_object.
 AgentIndividuals can be commanded to move to a destination. AgentIndividuals have a health, which
 is decreased when they take a hit. If the AgentIndividual's health > 0, it is alive.
 If its heath <= 0, it starts dying, then on subsequent updates,
 it becomes dead, and finally disappearing.
 */

#include "AgentComponent.h"
#include "Moving_object.h"

#include <memory>

class Structure;
struct Point;

class AgentIndividual :
  public AgentComponent, public Moving_object,
  public std::enable_shared_from_this<AgentIndividual> {
    
public:
	// create with initial health is 5, speed is 5, state is Alive
	AgentIndividual(const std::string& in_name, Point in_location);
	
  // class is abstract, make destructor pure virtual
  virtual ~AgentIndividual() = 0;
  
  std::shared_ptr<AgentIndividual> get_closest(Point origin) override;
  void get_closest_h(Point origin, std::shared_ptr<AgentIndividual> &best) override;
    
  // is this individual in range of annother point
  virtual bool in_range(Point point, double range) override;
  
	// return true if this agent is Alive
	bool is_alive() const { return alive; }
	
	// return this AgentIndividual's location
	Point get_location() const override;
  
	// return true if this AgentIndividual is in motion
	bool is_moving() const;
	
	// tell this AgentIndividual to start moving to location destination_
  void move_to(Point destination_) override;
  
	// tell this AgentIndividual to stop its activity
	void stop() override;
  
	// Tell this AgentIndividual to accept a hit from an attack of a specified strength
	// The attacking AgentIndividual identifies itself with its this pointer.
	// A derived class can override this function.
	// The function lose_health is called to handle the effect of the attack.
	virtual void take_hit(int attack_strength, std::shared_ptr<AgentIndividual> attacker_ptr);
  
  // Tell this AgentIndividual to accept a blessing from an agent specified by blesser_ptr.
  // A derived class may override this function, default behavior is empty.
  // The function gain_health may be called to handle the effect of the blessing.
  virtual void accept_blessing(int blessing_strength, std::shared_ptr<AgentIndividual> blesser_ptr) {}
	
	// update the moving state and AgentIndividual state of this object.
	void update() override;
	
	// output information about the current state
	void describe() const override;
	
	// ask Model to broadcast our current state to all Views
	void broadcast_current_state() override;
  
	// Throws exception that an AgentIndividual cannot work.
	void start_working(std::shared_ptr<Structure>, std::shared_ptr<Structure>) override;
  
	// Throws exception that an AgentIndividual cannot attack.
	void start_attacking(std::shared_ptr<AgentComponent>) override;
  
protected:
	// calculate loss of health due to hit.
	// if health decreases to zero or negative, AgentIndividual state becomes Dying, and any movement is stopped.
	void lose_health(int attack_strength);
  
  // calculate gain of health due to blessing. if health is already at full, or this agent is dead,
  // this method has no effect
  void gain_health(int blessing_strength);
  
private:
  
  int health;
  bool alive;
};


#endif