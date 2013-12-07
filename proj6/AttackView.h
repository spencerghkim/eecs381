#ifndef ATTACKVIEW_H_
#define ATTACKVIEW_H_

/*
 
 * AttackView class *
 
 This view will display a list of active duels. For 
 each agent the health and name will be displayed. The 
 distance between the agents is shown as well.
 
*/

#include "View.h"

#include <string>
#include <map>

class AttackView : public View { //TODO: is it worth trying to inherit from ListView?
public:
  
  // a new attack has started, keep track of it
  void update_attack(const std::string& name, const std::string& target) override;
  
  // an attack has ended, remove it
	void update_end_attack(const std::string& name) override;
  
  // an agent involved in an attack has lost health
  void update_health(const std::string& name, int health) override;
  
  // an agent involved in an attack has moved
  void update_location(const std::string& name, Point location) override;
  
  // Notify for removal of object
	virtual void update_remove(const std::string& name) override;
  
  // prints out the view
  void draw() override;
	
	// discards the view's contents
	void clear() override;
  
protected:
  using Attckers_t = std::map<std::string, std::string>;
  using Healths_t = std::map<std::string, int>;
  using Locations_t = std::map<std::string, Point>;
  
private:
  
  Attckers_t attackers;
  Healths_t healths;
  Locations_t locations;
};

#endif
