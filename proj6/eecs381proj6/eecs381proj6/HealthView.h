#ifndef HEALTHVIEW_H_
#define HEALTHVIEW_H_

/*
 
 * HealthView class *
 
 This class overrides the update_health method in order
 to save data in the ListView. It will also set the view
 name to "Health"
 
*/

#include "ListView.h"

class HealthView : public ListView {
public:
  
  // update the unit's health
  void update_health(const std::string& name, int health) override;
  
  // return the appropriate view name type
  std::string view_name() override;
};

#endif
