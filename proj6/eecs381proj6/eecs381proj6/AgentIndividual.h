#ifndef AGENTINDV_H_
#define AGENTINDV_H_

/*
 AgentIndividuals 
 
 This class represents and individal component and is the base class for all Agent types.
 It also inherits from Sim_object, and privately inherits from Moving_object.
 AgentIndividuals can be commanded to move to a destination. AgentIndividuals have a health, which
 is decreased when they take a hit. If the AgentIndividual's health > 0, it is alive.
 If its heath <= 0, it starts dying, then on subsequent updates,
 it becomes dead, and finally disappearing.
*/

#include "AgentComponent.h"
#include "Moving_object.h"
#include "Sim_object.h"

#include <memory>

class Structure;
struct Point;

class AgentIndividual :
  public AgentComponent, public Sim_object, public Moving_object,
  public std::enable_shared_from_this<AgentIndividual> {
    
public:
	
  // create with initial health is 5, speed is 5, state is Alive
	AgentIndividual(const std::string& in_name, Point in_location);
	
  // class is abstract, make destructor pure virtual
  virtual ~AgentIndividual() = 0;
    
  const std::string &get_name() const override
    { return Sim_object::get_name(); }

  // does this individual have this full name?
  bool has_name(const std::string& name_) override;

  // does this individual have this prefix?
  bool has_prefix(const std::string& prefix) override;

  // get shared_from_this
  std::shared_ptr<AgentIndividual> get_nearest_in_range(std::shared_ptr<const Sim_object> origin,
                                                        double range) override;

  // return a vector of only this object
  std::shared_ptr<AgentComponent> get_all_in_range(std::shared_ptr<const Sim_object> origin,
                                                   double range) override;
  
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

  // Tell this AgentIndividual to accept a blessing.
  // Subclasses may override, default implmentation is to do nothing.
  // Subclasses may use the gain_health method to handle the effects of a blessing.
  void accept_blessing(int blessing_strength, std::shared_ptr<AgentIndividual> blesser_ptr) override {}

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

  // adds individual back to model's base group
  void disband_from_group() override;

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
