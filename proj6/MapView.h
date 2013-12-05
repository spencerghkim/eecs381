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

#endif
