#ifndef FARM_H_
#define FARM_H_
/*
A Farm is a Structure that when updated, increments the amount of food on hand
by the production rate amount.
Food can be withdrawn, but no provision is made for depositing any.
*/

#include "Geometry.h"
#include "Structure.h"

class Farm : public Structure {
public:
	Farm (const std::string& in_name, Point in_location);
	
  ~Farm();
		
	// returns the specified amount, or the remaining amount, whichever is less,
	// and deducts that amount from the amount on hand
	double withdraw(double amount_to_get) override;

	// update adds the production amount to the stored amount
	void update() override;

	// output information about the current state
	void describe() const override;

private:
  double food_available;
};

#endif

