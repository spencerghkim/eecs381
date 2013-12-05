#ifndef LOCALMAPVIEW_H_
#define LOCALMAPVIEW_H_

/*
 
 * LocalMapView class *
 
 TODO: blurb
 
*/

#include "MapView.h"

class LocalMapView : public MapView {
public:
  LocalMapView(std::string name);
  
  // call MapView update_loc and move center if the name matches
  void update_location(const std::string& name, Point location) override;
  
  // print the headder for the local view
  void print_header() override;
  
  // don't print anything for off map
  void print_off_map(std::vector<std::string> &off) override {};
  
private:
  std::string obj_name;
};

#endif
