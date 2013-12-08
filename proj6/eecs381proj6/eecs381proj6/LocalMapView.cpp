
#include "LocalMapView.h"

#include <iostream>

using std::cout; using std::endl;
using std::string;

const double DEFAULT_LOCAL_SCALE = 2.0;
const int DEFAULT_LOCAL_SIZE = 9;
const double LOCAL_ORIGIN_OFFSET = (DEFAULT_LOCAL_SIZE / 2.0) * DEFAULT_LOCAL_SCALE; //TODO: is this correct *2/2 ?

LocalMapView::LocalMapView(string name) : obj_name(name)
{
  set_scale(DEFAULT_LOCAL_SCALE);
  set_size(DEFAULT_LOCAL_SIZE);
}

void LocalMapView::update_location(const string& name, Point location)
{
  MapView::update_location(name, location);
  if (name == obj_name) {
    location.x -= LOCAL_ORIGIN_OFFSET;
    location.y -= LOCAL_ORIGIN_OFFSET;
    set_origin(location);
  }
}

void LocalMapView::print_header()
{
  cout << "Local view for: " << obj_name << endl;
}


