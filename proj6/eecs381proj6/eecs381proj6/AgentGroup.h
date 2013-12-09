#ifndef AGENTGROUP_H_
#define AGENTGROUP_H_

/*
 
 AgentGroup
 
 TODO:
 
*/

#include "AgentComponent.h"

#include <map>
#include <functional>
#include <string>
#include <memory>

class AgentGroup : public AgentComponent {
public:
  
  AgentGroup(const std::string &name_);
  
  const std::string &get_name() const override
    { return name; };
  
  // iterate over the contained components and handle errors
  void iterate_and_catch(std::function<void(AgentComponent*)> func);
  
  // get the nearest agent in the group
  virtual std::shared_ptr<AgentIndividual> get_nearest(std::shared_ptr<const Sim_object> origin);
  
  // get the nearest agents in range
  std::vector<std::shared_ptr<AgentIndividual>> get_nearest_in_range(std::shared_ptr<const Sim_object> origin, double range) override;
  
  // is anyone in this group in range?
  bool in_range(Point point, double range) override;
  
	// tell this AgentComponent to start moving to location destination_
	void move_to(Point destination_) override;
  
	// tell this AgentComponent to stop its activity
	void stop() override;
  
	/* Fat Interface for derived classes */
  
	// Throws exception that an AgentComponent cannot work.
	void start_working(std::shared_ptr<Structure>, std::shared_ptr<Structure>) override;
  
	// Throws exception that an AgentComponent cannot attack.
	void start_attacking(std::shared_ptr<AgentComponent>) override;

  //Point get_location() const =delete; //TODO: hmm....
  
  // add component
  void add_component(std::shared_ptr<AgentComponent>) override;
  
  // remove component
  void remove_component(std::shared_ptr<AgentComponent>) override;
  
  // return the component of the specified name, empty if component does not exist
  std::shared_ptr<AgentComponent> get_component(const std::string& name_) override;
  
private:
  using Group_t = std::map<std::string, std::shared_ptr<AgentComponent>>;
  
  Group_t group_members;
  std::string name;
};

#endif
