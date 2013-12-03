#include "Peasant.h"

#include "Agent.h"
#include "Geometry.h"
#include "Model.h"
#include "Moving_object.h"
#include "Sim_object.h"
#include "Structure.h"
#include "Utility.h"

#include <iostream>

#define INITIAL_CARRYING_FOOD 0.0
#define MAXIMUM_CARRYING_CAPACITY 35.0

using std::cout; using std::endl;
using std::shared_ptr;

Peasant::Peasant(const std::string& in_name, Point in_location)
    : Agent(in_name, in_location),
      food_in_hand{INITIAL_CARRYING_FOOD},
      state{NOT_WORKING} {}

// implement Peasant behavior
void Peasant::update()
{
  Agent::update();
  
  if (!is_alive() || state == NOT_WORKING) {
    return;
  }
  
  if (state == INBOUND &&
      !is_moving() &&
      get_location() == source->get_location()) {
    state = COLLECTING;
  } else if (state == COLLECTING) {
    double request = MAXIMUM_CARRYING_CAPACITY - food_in_hand;
    double withdrawl = source->withdraw(request);
    food_in_hand += withdrawl;
    
    if (withdrawl > 0.0) {
      cout << get_name() << ": Collected " << withdrawl << endl;
      state = OUTBOUND;
      Agent::move_to(destination->get_location());
      
      // Let the Model know we've collected food.
      Model::getInstance().notify_amount(get_name(), food_in_hand);
    } else {
      cout << get_name() << ": Waiting " << endl;
    }
  } else if (state == OUTBOUND &&
             !is_moving() &&
             get_location() == destination->get_location()) {
    state = DEPOSITING;
  } else if (state == DEPOSITING) {
    destination->deposit(food_in_hand);
    cout << get_name() << ": Deposited " << food_in_hand << endl;
    food_in_hand = INITIAL_CARRYING_FOOD;
    Agent::move_to(source->get_location());
    state = INBOUND;
    
    // Let the model know we've deposited food.
    Model::getInstance().notify_amount(get_name(), food_in_hand);
  }
}

void Peasant::stop_working() {
  if (state != NOT_WORKING) {
    cout << get_name() << ": I'm stopping work" << endl;
    state = NOT_WORKING;
    destination = source = nullptr;
  }
}

// overridden to suspend working behavior
void Peasant::move_to(Point dest)
{
  stop_working();
  Agent::move_to(dest);
}

// stop moving and working
void Peasant::stop()
{
  Agent::stop();
  stop_working();
}

// starts the working process
// Throws an exception if the source is the same as the destination.
void Peasant::start_working(shared_ptr<Structure> source_, shared_ptr<Structure> destination_)
{
  Agent::stop();
  state = NOT_WORKING;
  destination = source = nullptr;
  
  if (source_ == destination_) {
    throw Error{ get_name() + ": I can't move food to and from the same place!" };
  }
  source = source_;
  destination = destination_;
  
  if (food_in_hand == 0.0) {
    if (get_location() == source->get_location()) {
      state = COLLECTING;
    } else {
      Agent::move_to(source->get_location());
      state = INBOUND;
    }
  } else {
    if (get_location() == destination->get_location()) {
      state = DEPOSITING;
    } else {
      Agent::move_to(destination->get_location());
      state = OUTBOUND;
    }
  }
}

// output information about the current state
void Peasant::describe() const
{
  cout << "Peasant ";
  Agent::describe();
  cout << "   Carrying " << food_in_hand << endl;
  
  if (state == OUTBOUND) {
    cout << "   Outbound to destination " << destination->get_name() << endl;
  } else if (state == INBOUND) {
    cout << "   Inbound to source " << source->get_name() << endl;
  } else if (state == COLLECTING) {
    cout << "   Collecting at source " << source->get_name() << endl;
  } else if (state == DEPOSITING) {
    cout << "   Depositing at destination " << destination->get_name() << endl;
  }
}
  
void Peasant::broadcast_current_state()
{
  Agent::broadcast_current_state();
  Model::getInstance().notify_amount(get_name(), food_in_hand);
}
