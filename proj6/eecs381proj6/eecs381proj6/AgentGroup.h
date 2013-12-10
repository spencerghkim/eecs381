#ifndef AGENTGROUP_H_
#define AGENTGROUP_H_

/*
 
 AgentGroup
 
 This class represents the composite state of an Agent Component.
 All actions taken on this component will be applied to each of the 
 components it holds, recursively.
 
*/

#include "AgentComponent.h"

#include <map>
#include <functional>
#include <string>
#include <memory>

class AgentGroup : public AgentComponent, public std::enable_shared_from_this<AgentGroup> {
public:
  
  AgentGroup(const std::string &name_);
  
  const std::string &get_name() const override
    { return group_name; }
  const std::string get_printed_name() const override
    { return "Group " + group_name; }
  
  // does any one in the group have this prefix?
  bool has_prefix(const std::string& prefix) override;
  
  // iterate over the contained components and handle errors
  void iterate_and_catch(std::function<void(AgentComponent*)> func);
  
  // get the nearest agent individual
  std::shared_ptr<AgentIndividual> get_nearest_in_range(std::shared_ptr<const Sim_object> origin,
                                                        double range) override;
  
  // get the nearest agent component within a range
  std::shared_ptr<AgentComponent> get_all_in_range(std::shared_ptr<const Sim_object> origin,
                                                   double range) override;
  
	// tell this AgentComponent to start moving to location destination_
	void move_to(Point destination_) override;
  
	// tell this AgentComponent to stop its activity
	void stop() override;
  
	/* Fat Interface for derived classes */
  
	// Throws exception that an AgentComponent cannot work.
	void start_working(std::shared_ptr<Structure>, std::shared_ptr<Structure>) override;
  
	// Throws exception that an AgentComponent cannot attack.
	void start_attacking(std::shared_ptr<AgentComponent>) override;

  // Accept a blessing. Calls accept_blessing for all group members.
  void accept_blessing(int blessing_strength, std::shared_ptr<AgentIndividual> blesser_ptr) override;

  // add component
  void add_component(std::shared_ptr<AgentComponent>) override;
  
  // return the component of the specified name, empty if component does not exist
  std::shared_ptr<AgentComponent> get_component(const std::string& name_) override;
  
  // remove component
  void remove_component(const std::string& name_) override;

  // remove component only if present, does not throw Error
  void remove_component_if_present(const std::string& name_) override;

  // emptys the group, default is an error
  void disband() override;
  
  // adds all individuals back to model's base
  void disband_from_group() override;
  
private:
  using Group_t = std::map<std::string, std::shared_ptr<AgentComponent>>;
  
  Group_t group_components;
  std::string group_name;
};

#endif
