#ifndef AGENTCOMPONENT_H_
#define AGENTCOMPONENT_H_

/*
 
 AgentIndividuals
 
 TODO:
 
*/

#include "Utility.h"
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
  
  virtual const std::string get_printed_name() const
      { return get_name(); }

  // get the nearest agent in the group, or none if no one is in range
  virtual std::shared_ptr<AgentIndividual> get_nearest_in_range(std::shared_ptr<const Sim_object> origin,
                                                                double range) = 0;
  
  // get the nearest agents in range
  virtual std::shared_ptr<AgentComponent> get_all_in_range(std::shared_ptr<const Sim_object> origin,
                                                           double range) = 0;
  
	// tell this AgentComponent to start moving to location destination_
	virtual void move_to(Point destination_) = 0;
  
	// tell this AgentComponent to stop its activity
	virtual void stop() = 0;
  
	/* Fat Interface for derived classes */
  
	// tell the component to start working
	virtual void start_working(std::shared_ptr<Structure>, std::shared_ptr<Structure>) = 0;
  
	// tell the component to start attacking
	virtual void start_attacking(std::shared_ptr<AgentComponent>) = 0;
  
  // Tell this AgentComponent to accept a blessing from an agent specified by blesser_ptr.
  // A derived class must override this function.
  virtual void accept_blessing(int blessing_strength, std::shared_ptr<AgentIndividual> blesser_ptr) = 0;
  
  // add component, throws "Cannot add" Error as default
  virtual void add_component(std::shared_ptr<AgentComponent>)
    { throw Error("This agent component cannot add components!"); }

  // return the component of the specified name, returns nullptr as default
  virtual std::shared_ptr<AgentComponent> get_component(const std::string& name_)
    { return {nullptr}; }

  // remove component, throws "Cannot remove" Error as default
  virtual void remove_component(const std::string& name_)
    { throw Error("This agent component cannot remove components!"); }
  
  // TODO: fix this shit
  // remove component, but dont throw
  virtual void remove_component_if_present(const std::string& name_) {};
};

#endif
