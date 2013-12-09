#ifndef VIEW_H_
#define VIEW_H_

/*
 
 * View class *
 
 The View class is an interface class which a view of a specific
 type must follow in order to render correctly.
 
*/

#include <string>
#include "Geometry.h"

class View { //Interface for any type of View
public:
  virtual ~View() = 0;
  
	// Notify for location, health, or amount update
	virtual void update_location(const std::string& name, Point location) {};
	virtual void update_health(const std::string& name, int health) {};
	virtual void update_amount(const std::string& name, double amount) {};
  
  // Notify for begin or end of an attack
	virtual void update_attack(const std::string& name, const std::string& target) {};
	virtual void update_end_attack(const std::string& name) {};
	
	// Notify for removal of object
	virtual void update_remove(const std::string& name) = 0;
	
	// prints out the view
	virtual void draw() = 0;
	
	// discards the view's contents
	virtual void clear() = 0;
};

#endif
