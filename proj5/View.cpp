#include "View.h"

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

#define DEFAULT_MAP_SIZE 25
#define DEFAULT_SCALE 2.0
#define DEFAULT_ORIGIN_X -10.0
#define DEFAULT_ORIGIN_Y -10.0

#define EMPTY_CELL_STRING ". "
#define MULTIPLE_OCCUPANTS_CELL_STRING "* "

#define GRID_AXIS_LABEL_STEP 3

using Grid = vector< vector <string> >;
using Locations_c = map<string, Point>;

View::View()
{
  set_defaults();
}

// Save the supplied name and location for future use in a draw() call
// If the name is already present,the new location replaces the previous one.
void View::update_location(const std::string& name, Point location)
{
  locations[name] = location;
}

// Remove the name and its location; no error if the name is not present.
void View::update_remove(const std::string& name)
{
  locations.erase(name);
}

// prints out the current map
void View::draw()
{
  // Print metrics.
  cout << "Display size: " << size << ", scale: " << scale << ", origin: " << origin << endl;
  
  // Create the actual grid.
  vector<string> row(size, EMPTY_CELL_STRING);
  Grid grid(size, row);
  
  // Populate grid and objects off screen.
  vector<string> objects_off_screen;
  std::for_each(locations.begin(), locations.end(), [&] (Locations_c::value_type pair) {
    int ix, iy;
    if (this->get_subscripts(ix, iy, pair.second)) {
      if (grid[iy][ix] == EMPTY_CELL_STRING) {
        grid[iy][ix] = pair.first.substr(0, UNIQUE_STRING_PREFIX_SIZE);
      } else {
        grid[iy][ix] = MULTIPLE_OCCUPANTS_CELL_STRING;
      }
    } else {
      objects_off_screen.push_back(pair.first);
    }
  });
  
  // Output objects off screen.
  if (!objects_off_screen.empty()) {
    cout << *objects_off_screen.begin();
    std::for_each(std::next(objects_off_screen.begin()),
                  objects_off_screen.end(),
                  [] (const string& name) {
                      cout << ", " << name;
                  });
    cout << " outside the map" << endl;
  }
 
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
void View::clear()
{
  locations.clear();
}

// modify the display parameters
// if the size is out of bounds will throw Error("New map size is too big!")
// or Error("New map size is too small!")
void View::set_size(int size_)
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
void View::set_scale(double scale_)
{
  if (scale_ <= 0.0) {
    throw Error{"New map scale must be positive!"};
  }
  scale = scale_;
}

// any values are legal for the origin
void View::set_origin(Point origin_)
{
  origin = origin_;
}

// set the parameters to the default values
void View::set_defaults()
{
  size = DEFAULT_MAP_SIZE;
  scale = DEFAULT_SCALE;
  origin.x = DEFAULT_ORIGIN_X;
  origin.y = DEFAULT_ORIGIN_Y;
}


// Calculate the cell subscripts corresponding to the supplied location parameter,
// using the current size, scale, and origin of the display. 
// This function assumes that origin is a  member variable of type Point, 
// scale is a double value, and size is an integer for the number of rows/columns 
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool View::get_subscripts(int &ix, int &iy, Point location)
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

