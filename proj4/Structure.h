#ifndef STRUCTURE_H_
#define STRUCTURE_H_
/* A Structure is a Sim_object with a location and interface to derived types */

#include "Sim_object.h"

#include <string>

struct Point;

class Structure : public Sim_object {
public:
  Structure(const std::string& name_, Point location_);
	
	// Make this an abstract class by making the destructor pure virtual
  virtual ~Structure() = 0;
		
	// *** declare and define here appropriately
	Point get_location() const { return location; }

	// *** declare and define the following functions as specified
	void update() override {}

	// output information about the current state
	void describe() const override;

	// ask model to notify views of current state
  void broadcast_current_state() override;
    
	// fat interface for derived types	
	virtual double withdraw(double amount_to_get) { return 0.0; }
	virtual void deposit(double amount_to_give) {}
  
private:
  Point location;
};

#endif
