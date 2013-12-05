#ifndef MODEL_H_
#define MODEL_H_

/*
 
 Model is part of a simplified Model-View-Controller pattern.
 Model keeps track of the Sim_objects in our little world. It is the only
 component that knows how many Structures and Agents there are, but it does not
 know about any of their derived classes, nor which Agents are of what kind of Agent.
 It has facilities for looking up objects by name, and removing Agents.  When
 created, it creates an initial group of Structures and Agents using the Structure_factory
 and Agent_factory.
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
class Agent;
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
  
	// is there a structure with this name?
	bool is_structure_present(const std::string& name) const;
	// add a new structure; assumes none with the same name
	void add_structure(std::shared_ptr<Structure>);
	// will throw Error("Structure not found!") if no structure of that name
	std::shared_ptr<Structure> get_structure_ptr(const std::string& name) const;
	// returns the closest structure to the provided agent
	std::shared_ptr<Structure> closest_structure(std::shared_ptr<Sim_object> object) const;
  
	// is there an agent with this name?
	bool is_agent_present(const std::string& name) const;
	// add a new agent; assumes none with the same name
	void add_agent(std::shared_ptr<Agent>);
  // remove an agent
  void remove_agent(std::shared_ptr<Agent>);
  
	// will throw Error("Agent not found!") if no agent of that name
	std::shared_ptr<Agent> get_agent_ptr(const std::string& name) const;
	// returns the closest agent to the provided agent (not the same agent)
	std::shared_ptr<Agent> closest_agent(std::shared_ptr<Sim_object> object) const;
  // find all agents in the given range around the given object
  std::vector<std::shared_ptr<Agent>> find_agents_in_range(std::shared_ptr<Sim_object> center, double range);
  
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
  // notify the views about an attack
  void notify_attack(const std::string& name, const std::string& target);
  
  // notify the views that an object is now gone
	void notify_gone(const std::string& name);
  // notify the views about the end of an attack
  void notify_end_attack(const std::string& name);

private:
  using Objects_t = std::map<std::string, std::shared_ptr<Sim_object>>;
  using Structures_t = std::map<std::string, std::shared_ptr<Structure>>;
  using Agents_t = std::map<std::string, std::shared_ptr<Agent>>;
  
  // Insert into containters w/o broadcasting
  void insert_structure(std::shared_ptr<Structure>);
  void insert_agent(std::shared_ptr<Agent>);
  
  int time;
  Objects_t objects;
  Structures_t structures;
  Agents_t agents;
  std::set<std::shared_ptr<View>> views;
  
	// disallow copy/move construction or assignment
	Model(const Model&)             = delete;
	Model& operator= (const Model&) = delete;
	Model(Model&&)                  = delete;
	Model& operator= (Model&&)      = delete;
};

#endif