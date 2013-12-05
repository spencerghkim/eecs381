#ifndef FULLMAPVIEW_H_
#define FULLMAPVIEW_H_

/*
 
 * FullMapView class *
 
 TODO: blurb
 
*/

#include "MapView.h"

class FullMapView : public MapView {
public:
  // default constructor sets the default size, scale, and origin
	FullMapView();
  
  // forward private functions
	void set_size(int size_)
    { MapView::set_size(size_); }
	void set_scale(double scale_)
    { MapView::set_scale(scale_); }
  void set_origin(Point origin_)
    { MapView::set_origin(origin_); }
	void set_defaults()
    { MapView::set_defaults(); }
};

#endif
