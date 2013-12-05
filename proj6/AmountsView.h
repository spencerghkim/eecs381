#ifndef AMOUNTSVIEW_H_
#define AMOUNTSVIEW_H_

/*
 
 * AmountsView class *
 
 TODO: blurb
*/

#include "ListView.h"

class AmountsView : public ListView {
public:
  // update the amount of food
  void update_amount(const std::string& name, double amount) override;
  std::string view_name() override;
};

#endif
