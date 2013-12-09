#ifndef AGENTCOMPONENT_H_
#define AGENTCOMPONENT_H_

/*
 
 AgentIndividuals
 
 TODO:
 
*/

#include "Geometry.h"

#include <string>
#include <memory>

class AgentIndividual;
class Structure;

class AgentComponent {
public:

  // class is abstract, make destructor pure virtual
  virtual ~AgentComponent() = 0;
  
  virtual const std::string &get_name() const = 0;

  virtual std::shared_ptr<AgentIndividual> get_closest(Point origin) = 0; //TODO: protected _h?
  virtual void get_closest_h(Point origin, std::shared_ptr<AgentIndividual> &best) = 0;
  
  // is this component in range?
  virtual bool in_range(Point point, double range) = 0;
  
	// tell this AgentComponent to start moving to location destination_
	virtual void move_to(Point destination_) = 0;
  
	// tell this AgentComponent to stop its activity
	virtual void stop() = 0;
  
	/* Fat Interface for derived classes */
  
	// Throws exception that an AgentComponent cannot work.
	virtual void start_working(std::shared_ptr<Structure>, std::shared_ptr<Structure>) = 0;
  
	// returns true if 
	virtual void start_attacking(std::shared_ptr<AgentComponent>) = 0;

};

#endif
