#ifndef MAPVIEW_H_
#define MAPVIEW_H_

/*
 
 * MapView class *
 
 TODO: blurb
 
*/

#include "View.h"
#include "Geometry.h"

#include <string>
#include <map>
#include <vector>
#include <memory>

class MapView : public View {
public:
  virtual ~MapView() = 0;
  
  // Save the supplied name and location for future use in a draw() call
	// If the name is already present,the new location replaces the previous one.
	void update_location(const std::string& name, Point location) override;
	
	// Remove the name and its location; no error if the name is not present.
	void update_remove(const std::string& name) override;
  
	// Discard the saved information - drawing will show only a empty pattern
	void clear() override;
  
protected:
  using Objects_t = std::map<std::string, Point>;
  using Grid_t = std::vector<std::vector<std::string>>;
  
  // prints out the current map
	void draw() override;
  // default header print, called during draw
  virtual void print_header();
  // override to handle object out of range of the bounds
  virtual void print_off_map(std::vector<std::string> &off);
  
  // modify the display parameters
	void set_size(int size_);
	void set_scale(double scale_);
	void set_origin(Point origin_);
	void set_defaults();
  
private:
  Grid_t make_grid(int size);
  void init_grid_data(Grid_t &grid);
  void print_grid(const Grid_t &grid);
  bool get_subscripts(int &ix, int &iy, Point location) const;
  
  Objects_t objects;
  int size;
  double scale;
  Point origin;
};

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

class LocalView : public MapView {
public:
  LocalView(std::string name);
  
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
