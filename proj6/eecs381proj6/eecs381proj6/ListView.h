#ifndef LISTVIEW_H_
#define LISTVIEW_H_

/*
 
 * ListView class *
 
 This view is capable of rendering a list of doubles
 which correspond to some statistic about an observed 
 object.
 
*/

#include "View.h"

#include <string>
#include <map>

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

#endif
