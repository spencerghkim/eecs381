#ifndef PEASANT_H_
#define PEASANT_H_

/*
A Peasant is an Agent that can move food between Structures. It can be commanded to
start_working, whereupon it moves to the source, picks up food, returns to destination,
deposits the food, returns to source.  If picks up zero food at the source, it waits there
and tries again on the next update. 
If commanded to move_to somewhere, it stops working, and goes there.
*/

#include "Agent.h"
#include "Moving_object.h"
#include "Sim_object.h"

#include <memory>

class Structure;
struct Point;

class Peasant : public Agent {
public:
	// *** define these in .cpp; initialize with zero amount being carried
	Peasant(const std::string& in_name, Point in_location);

	~Peasant();

	// implement Peasant behavior
	void update() override;
	
	// overridden to suspend working behavior
  void move_to(Point dest) override;
	
	// stop moving and working
	void stop() override;

	// starts the working process
	// Throws an exception if the source is the same as the destination.
	void start_working(std::shared_ptr<Structure> source_, std::shared_ptr<Structure> destination_) override;

	// output information about the current state
	void describe() const override;
  
  // ask model to broadcast our current state
  void broadcast_current_state() override;
  
private:
  typedef enum {
    NOT_WORKING,
    INBOUND,
    COLLECTING,
    OUTBOUND,
    DEPOSITING
  } Peasant_state_e;
  
  double food_in_hand;
  std::shared_ptr<Structure> source;
  std::shared_ptr<Structure> destination;
  Peasant_state_e state;
  
  void stop_working();
};

#endif
