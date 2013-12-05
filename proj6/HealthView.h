#ifndef HEALTHVIEW_H_
#define HEALTHVIEW_H_

/*
 
 * HealthView class *
 
 TODO: blurb
*/

#include "ListView.h"

class HealthView : public ListView {
public:
  // update the unit's health
  void update_health(const std::string& name, int health) override;
  std::string view_name() override;
};

#endif
