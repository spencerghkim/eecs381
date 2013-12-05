#ifndef VIEW_H_
#define VIEW_H_

/*
 
 * View class *
 
 The View classes encapsulates the data and functions needed to generate the correct
 displays, and control their properties. It has a "memory" for the names and locations
 of the to-be-plotted objects.
 
*/

#include <string>
#include "Geometry.h"

class View { //Interface for any type of View
public:
  //TODO: check that we need this.
  virtual ~View() = 0;
  
	// Notify for location, health, or amount update
	virtual void update_location(const std::string& name, Point location) {};
	virtual void update_health(const std::string& name, int health) {};
	virtual void update_amount(const std::string& name, double amount) {};
	
	// Notify for removal of object
	virtual void update_remove(const std::string& name) = 0;
	
	// prints out the view
	virtual void draw() = 0;
	
	// discards the view's contents
	virtual void clear() = 0;
};

#endif
