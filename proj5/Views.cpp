#include "Views.h"

#include "Geometry.h"
#include "Utility.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using std::map; using std::vector; using std::string;
using std::cout; using std::endl;

#define MAP_SIZE_MAX 30
#define MAP_SIZE_MIN 7

#define DEFAULT_MAP_VIEW_SIZE 25
#define DEFAULT_MAP_VIEW_SCALE 2.0
#define DEFAULT_MAP_VIEW_ORIGIN_X -10.0
#define DEFAULT_MAP_VIEW_ORIGIN_Y -10.0

#define LOCAL_VIEW_SIZE 9
#define LOCAL_VIEW_SCALE 2.0

#define EMPTY_CELL_STRING ". "
#define MULTIPLE_OCCUPANTS_CELL_STRING "* "

#define GRID_AXIS_LABEL_STEP 3

#define AMOUNTS_VIEW_NAME "Amounts"
#define HEALTH_VIEW_NAME "Health"

using Grid = vector< vector <string> >;
using Locations_c = map<string, Point>;

// Default destructor for View.
View::~View() {}

// Default destructor for LocationView.
LocationView::~LocationView() {}

// Save the supplied name and location for future use in a draw() call
// If the name is already present,the new location replaces the previous one.
void LocationView::update_location(const std::string& name, Point location)
{
  locations[name] = location;
}

// Remove the name and its location; no error if the name is not present.
void LocationView::update_remove(const std::string& name)
{
  locations.erase(name);
}

// prints out the current map
void LocationView::draw()
{
  // Create the actual grid.
  vector<string> row(size, EMPTY_CELL_STRING);
  Grid grid(size, row);
  
  // Populate grid.
  std::for_each(locations.begin(), locations.end(), [&] (Locations_c::value_type pair) {
    int ix, iy;
    if (this->get_subscripts(ix, iy, pair.second)) {
      if (grid[iy][ix] == EMPTY_CELL_STRING) {
        grid[iy][ix] = pair.first.substr(0, UNIQUE_STRING_PREFIX_SIZE);
      } else {
        grid[iy][ix] = MULTIPLE_OCCUPANTS_CELL_STRING;
      }
    }
  });
 
  int old_precision = cout.precision(); 
  cout.precision(0);
  // Print the y-axis lables and the grid.
  for (int y = grid.size() - 1; y >= 0; --y) {
    if (y % GRID_AXIS_LABEL_STEP == 0) {
      cout << std::setw(4) << origin.y + (scale * y) << " ";
    } else {
      cout << "     ";
    }
    for (auto& point : grid[y]) {
      cout << point;
    }
    cout << endl;
  }
  // Print the x-axis labels.
  for (int x_step = 0; x_step < (size - 1)/GRID_AXIS_LABEL_STEP + 1; ++x_step) {
    cout << "  " << std::setw(4) << origin.x + x_step*GRID_AXIS_LABEL_STEP*scale;
  }
  cout << endl;

  cout.precision(old_precision);
}

// Discard the saved information - drawing will show only a empty pattern
void LocationView::clear()
{
  locations.clear();
}

// modify the display parameters
// if the size is out of bounds will throw Error("New map size is too big!")
// or Error("New map size is too small!")
void LocationView::set_size(int size_)
{
  if (size_ > MAP_SIZE_MAX) {
    throw Error{"New map size is too big!"};
  }
  if (size_ < MAP_SIZE_MIN) {
    throw Error{"New map size is too small!"};
  }
  size = size_;
}

// If scale is not postive, will throw Error("New map scale must be positive!");
void LocationView::set_scale(double scale_)
{
  if (scale_ <= 0.0) {
    throw Error{"New map scale must be positive!"};
  }
  scale = scale_;
}

// any values are legal for the origin
void LocationView::set_origin(Point origin_)
{
  origin = origin_;
}

void LocationView::set_center(Point center)
{
  origin.x = center.x - (size/2)*scale;
  origin.y = center.y - (size/2)*scale;
}

// Service for derived views.
void LocationView::print_metrics() const
{
  // Print metrics.
  cout << "Display size: " << size << ", scale: " << scale << ", origin: " << origin << endl;
}

// Service for derived views.
void LocationView::print_objects_offscreen() const
{
  // Populate objects off screen.
  vector<string> objects_off_screen;
  std::for_each(locations.begin(), locations.end(), [&] (Locations_c::value_type pair) {
    int ix, iy;
    if (!(this->get_subscripts(ix, iy, pair.second))) {
      objects_off_screen.push_back(pair.first);
    }
  });
  
  // Print objects off screen.
  if (!objects_off_screen.empty()) {
    cout << objects_off_screen[0];
    for (int i = 1; i < objects_off_screen.size(); ++i) {
      cout << ", " << objects_off_screen[i];
    }
    cout << " outside the map" << endl;
  }
}

// Calculate the cell subscripts corresponding to the supplied location parameter,
// using the current size, scale, and origin of the display.
// This function assumes that origin is a  member variable of type Point,
// scale is a double value, and size is an integer for the number of rows/columns
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool LocationView::get_subscripts(int &ix, int &iy, Point location) const
{
	// adjust with origin and scale
	Cartesian_vector subscripts = (location - origin) / scale;
	// truncate coordinates to integer after taking the floor
	// floor function will return the largest integer smaller than the supplied value
	// even for negative values, so -0.05 => -1., which will be outside the array.
	ix = int(floor(subscripts.delta_x));
	iy = int(floor(subscripts.delta_y));
	// if out of range, return false
	if ((ix < 0) || (ix >= size) || (iy < 0) || (iy >= size)) {
		return false;
  }
	else
		return true;
}

MapView::MapView(const string& name_) : LocationView(name_)
{
  set_defaults();
}

void MapView::draw()
{
  // Print the scale, origin, etc..
  print_metrics();
  print_objects_offscreen();
  
  // Draw the grid view.
  LocationView::draw();
}

// set the parameters to the default values
void MapView::set_defaults()
{
  set_size(DEFAULT_MAP_VIEW_SIZE);
  set_scale(DEFAULT_MAP_VIEW_SCALE);
  set_origin(Point(DEFAULT_MAP_VIEW_ORIGIN_X, DEFAULT_MAP_VIEW_ORIGIN_Y));
}

LocalView::LocalView(const string& name_) : LocationView(name_)
{
  set_size(LOCAL_VIEW_SIZE);
  set_scale(LOCAL_VIEW_SCALE);
}

void LocalView::update_location(const std::string &name, Point location)
{
  if (name == get_name()) {
    set_center(location);
  }
}

void LocalView::draw()
{
  cout << "Local view for: " << get_name() << endl;
  
  // Draw the grid view.
  LocationView::draw();
}

// Default constructor for AttributeView.
AttributeView::~AttributeView() {}

void AttributeView::update_remove(const std::string &name)
{
  attributes.erase(name);
}

// Draw the 'table' of current attributes.
void AttributeView::draw()
{
  int old_precision = cout.precision();
  
  cout << "Current " << get_attribute_name() << ":" << endl;
  cout << "--------------" << endl;
  for (auto attribute_pair : attributes) {
    cout << attribute_pair.first << ": " << attribute_pair.second << endl;
  }
  cout << "--------------" << endl;

  cout.precision(old_precision);
}

void AttributeView::clear()
{
  attributes.clear();
}

void AttributeView::set_attribute(const std::string &name, double attribute)
{
  attributes[name] = attribute;
}

void HealthView::update_health(const std::string &name, double health)
{
  set_attribute(name, health);
}

string HealthView::get_attribute_name()
{
  return HEALTH_VIEW_NAME;
}

void AmountView::update_amount(const std::string &name, double amount)
{
  set_attribute(name, amount);
}

string AmountView::get_attribute_name()
{
  return AMOUNTS_VIEW_NAME;
}




