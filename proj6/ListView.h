#ifndef LISTVIEW_H_
#define LISTVIEW_H_

/*
 
 * ListView class *
 
 TODO: blurb
*/

#include "View.h"
#include "Geometry.h"

#include <string>
#include <map>

/* List View Types */

class ListView : public View {
public:
  virtual ~ListView() = 0;
  
  // Notify for removal of object
  void update_remove(const std::string& name) override;
	
	// prints out the view
	void draw() override;
	
	// discards the view's contents
	void clear() override;
  
protected:
  using Objects_t = std::map<std::string, double>;
  
  // update the generic value held by the ListView
  void update_value(const std::string& name, double value);
  
private:
  // get the name of this view type
  virtual std::string view_name() = 0;
  
  Objects_t objects;
};

class HealthView : public ListView {
public:
  // update the unit's health
  void update_health(const std::string& name, int health) override;
  std::string view_name() override;
};

class AmountsView : public ListView {
public:
  // update the amount of food
  void update_amount(const std::string& name, double amount) override;
  std::string view_name() override;
};

#endif
