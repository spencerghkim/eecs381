#ifndef AMOUNTSVIEW_H_
#define AMOUNTSVIEW_H_

/*
 
 * AmountsView class *
 
 This class overrides the update_amount method in order
 to save data in the ListView. It will also set the view
 name to "Amounts"
 
*/

#include "ListView.h"

class AmountsView : public ListView {
public:
  
  // update the amount of food
  void update_amount(const std::string& name, double amount) override;
  
  // return the appropriate view name type
  std::string view_name() override;
};

#endif
