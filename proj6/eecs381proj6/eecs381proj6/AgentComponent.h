#ifndef AGENTCOMPONENT_H_
#define AGENTCOMPONENT_H_

/*
 
 AgentIndividuals
 
 TODO:
 
*/

#include "Geometry.h"

#include <string>
#include <memory>
#include <vector>

class AgentIndividual;
class Sim_object;
class Structure;

class AgentComponent {
public:

  // class is abstract, make destructor pure virtual
  virtual ~AgentComponent() = 0;
  
  // all individuals and groups must have a name
  virtual const std::string &get_name() const = 0;

  // get the nearest agent in the group (by default the single agent for individuals)
  virtual std::shared_ptr<AgentIndividual> get_nearest(std::shared_ptr<const Sim_object> origin);
  
  // get the nearest agents in range
  virtual std::vector<std::shared_ptr<AgentIndividual>> get_nearest_in_range(std::shared_ptr<const Sim_object> origin, double range);
  
  // is this component in range?
  virtual bool in_range(Point point, double range) = 0;
  
	// tell this AgentComponent to start moving to location destination_
	virtual void move_to(Point destination_) = 0;
  
	// tell this AgentComponent to stop its activity
	virtual void stop() = 0;
  
	/* Fat Interface for derived classes */
  
	// Throws exception that an AgentComponent cannot work.
	virtual void start_working(std::shared_ptr<Structure>, std::shared_ptr<Structure>) = 0;
  
	// tell the component to start attacking
	virtual void start_attacking(std::shared_ptr<AgentComponent>) = 0;
  
  // add component, does nothing for individuals
  virtual void add_component(std::shared_ptr<AgentComponent>) {}
  
  // remove component, does nothing for individuals
  virtual void remove_component(const std::string& name_) {}
  
  // return the component of the specified name, empty if component does not exist
  virtual std::shared_ptr<AgentComponent> get_component(const std::string& name_)
    {return {nullptr};}
  
};

#endif
