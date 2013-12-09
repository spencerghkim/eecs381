#ifndef MODEL_H_
#define MODEL_H_

/*
 
 Model is part of a simplified Model-View-Controller pattern.
 Model keeps track of the Sim_objects in our little world. It is the only
 component that knows how many Structures and AgentComponents there are, but it does not
 know about any of their derived classes, nor which AgentComponents are of what kind of AgentComponent.
 It has facilities for looking up objects by name, and removing AgentComponents.  When
 created, it creates an initial group of Structures and AgentComponents using the Structure_factory
 and AgentComponent_factory.
 Finally, it keeps the system's time.
 
 Controller tells Model what to do; Model in turn tells the objects what do, and
 when asked to do so by an object, tells all the Views whenever anything changes that might be relevant.
 Model also provides facilities for looking up objects given their name.
 
 Notice how only the Standard Library headers need to be included - reduced coupling!
 
 */

#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>

class Sim_object;
class Structure;
class AgentComponent;
class AgentIndividual;
class AgentGroup;
class View;
struct Point;
class Model;

class Model {
  
  // create the initial objects
	Model();
  
public:
  
  // return singleton instance of model
  static Model& get();
  
	// return the current time
	int get_time() {return time;}
  
	// is name already in use for either agent or structure?
  // either the identical name, or identical in first two characters counts as in-use
	bool is_name_in_use(const std::string& name) const;
  // check if the full name given matches an existing object
  bool object_fullname_exists(const std::string &name) const;
  
	// is there a structure with this name?
	bool is_structure_present(const std::string& name) const;
	// add a new structure; assumes none with the same name
	void add_structure(std::shared_ptr<Structure>);
	// will throw Error("Structure not found!") if no structure of that name
	std::shared_ptr<Structure> get_structure_ptr(const std::string& name) const;
	// returns the closest structure to the provided agent
	std::shared_ptr<Structure> closest_structure(std::shared_ptr<Sim_object> object) const;
  
	// is there an agent with this name?
	bool is_agent_component_present(const std::string& name) const;
  // is this component in a group?
  bool is_agent_component_in_group(std::shared_ptr<AgentComponent>) const;
	// add a new agent component; throws if name is already in use
	void add_agent_component(std::shared_ptr<AgentComponent>);
  // removes the specified agent component, throws if doesn't exist
  void remove_agent_component(const std::string& name);

  // add a new individual agent, throws if name is in use
  void add_new_agent(std::shared_ptr<AgentIndividual>);
  // removes an agent from Views, Sim_objects, and AgentComponents; throws if doesn't exist
  void remove_agent(const std::string& name);
  
	// will throw Error("AgentComponent not found!") if no agent of that name
	std::shared_ptr<AgentComponent> get_agent_comp_ptr(const std::string& name) const;
	// returns the closest agent to the provided agent (not the same agent)
	std::shared_ptr<AgentComponent> closest_agent_in_range(std::shared_ptr<Sim_object> object,
                                                          double range) const;
  // find all agents in the given range around the given object
  std::shared_ptr<AgentComponent> find_agents_in_range(std::shared_ptr<Sim_object> center,
                                                       double range) const;
  
	// tell all objects to describe themselves to the console
	void describe() const;
	// increment the time, and tell all objects to update themselves
	void update();
	
	/* View services */
  
	// Attaching a View adds it to the container and causes it to be updated
  // with all current objects'location (or other state information.
	void attach(std::shared_ptr<View>);
	// Detach the View by discarding the supplied pointer from the container of Views
  // - no updates sent to it thereafter.
	void detach(std::shared_ptr<View>);
  
  // notify the views about an object's location
	void notify_location(const std::string& name, Point location);
  // notify the views about an object's health
  void notify_health(const std::string& name, int health);
  // notify the views about an object's amount
  void notify_amount(const std::string& name, double amount);
  // notify the views that an object is now gone
	void notify_gone(const std::string& name);
  // notify the views about an attack
  void notify_attack(const std::string& name, const std::string& target);
  // notify the views about the end of an attack
  void notify_end_attack(const std::string& name);

private:
  using Objects_t = std::map<std::string, std::shared_ptr<Sim_object>>;
  using Structures_t = std::map<std::string, std::shared_ptr<Structure>>;
  using AgentComponents_t = std::map<std::string, std::shared_ptr<AgentComponent>>;
  
  // Insert into containters w/o broadcasting
  void insert_structure(std::shared_ptr<Structure>);
  void insert_new_agent(std::shared_ptr<AgentIndividual>);
  
  int time;
  Objects_t objects;
  Structures_t structures;
  std::unique_ptr<AgentGroup> all_agents;
  
  std::set<std::shared_ptr<View>> views;
  
	// disallow copy/move construction or assignment
	Model(const Model&)             = delete;
	Model& operator= (const Model&) = delete;
	Model(Model&&)                  = delete;
	Model& operator= (Model&&)      = delete;
};

#endif