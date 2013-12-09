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
  
  // iterate over the contained components and handle errors
  void iterate_and_catch(std::function<void(AgentComponent*)> func);
  
  std::shared_ptr<AgentIndividual> get_closest(Point origin) override;
  void get_closest_h(Point origin, std::shared_ptr<AgentIndividual> &best) override;
  
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
  
private:
  using Group_t = std::map<std::string, std::shared_ptr<AgentComponent>>;
  
  Group_t group_members;
};

#endif
