
#include "MapView.h"

#include "Geometry.h"
#include "Utility.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>

using std::cout; using std::endl;
using std::setw; using std::setprecision;
using std::string; using std::vector;
using std::shared_ptr;
using std::make_shared;

const int DEFAULT_SIZE = 25;
const double MAX_SIZE = 30.0;
const double MIN_SIZE = 6.0;
const double DEFAULT_SCALE = 2.0;
const Point DEFAULT_ORIGIN {-10.0,-10.0};

const string BLANK_CORDINATE = ". ";
const string MULTI_CORDINATE = "* ";

// explicit default dtor
MapView::~MapView() {}

// Save the supplied name and location for future use in a draw() call
// If the name is already present,the new location replaces the previous one.
void MapView::update_location(const string& name, Point location)
{
  objects[name] = location;
}

// Remove the name and its location; no error if the name is not present.
void MapView::update_remove(const string& name)
{
  auto itr = objects.find(name);
  if (itr != objects.end()) objects.erase(itr);
}

// Discard the saved information - drawing will show only a empty pattern
void MapView::clear()
{
  objects.clear();
}

// modify the display parameters
// if the size is out of bounds will throw
void MapView::set_size(int size_)
{
  if (size_ > MAX_SIZE) {
    throw Error("New map size is too big!");
  } else if (size_ <= MIN_SIZE) {
    throw Error("New map size is too small!");
  }
  size = size_;
}

// If scale is not postive, will throw Error("New map scale must be positive!");
void MapView::set_scale(double scale_)
{
  if (scale_ <= 0.0) {
    throw Error("New map scale must be positive!");
  }
  scale = scale_;
}

// any values are legal for the origin
void MapView::set_origin(Point origin_)
{
  origin = origin_;
}

// set the parameters to the default values
void MapView::set_defaults()
{
  size = DEFAULT_SIZE;
  scale = DEFAULT_SCALE;
  origin = DEFAULT_ORIGIN;
}

void MapView::draw()
{
  Grid_t grid;
  print_header();
  init_grid_data(grid);
  print_grid(grid);
}

void MapView::print_header()
{
  cout << "Display size: " << size << ", scale: ";
  cout << scale << ", origin: " << origin << endl;
}

void MapView::print_off_map(vector<string> &off)
{
  for (auto itr = off.begin(); itr != off.end(); itr++) {
    if (itr != off.begin()) {
      cout << ", ";
    }
    cout << *itr;
  }
  if (off.size()) {
    cout << " outside the map" << endl;
  }
}

MapView::Grid_t MapView::make_grid(int size)
{
  Grid_t grid(size);
  for (auto &row : grid) {
    row.resize(size);
    for (auto &col : row) {
      col = BLANK_CORDINATE;
    }
  }
  return grid;
}

void MapView::init_grid_data(Grid_t &grid)
{
  // setup grid
  grid = make_grid(size);
  
  // init cells of grid
  vector<string> off_map;
  for (auto &i : objects) {
    int ix, iy;
    if(get_subscripts(ix, iy, i.second)) {
      if(grid[iy][ix] == BLANK_CORDINATE) {
        grid[iy][ix] = i.first.substr(0,2);
      } else {
        // unit is already here, show *
        grid[iy][ix] = MULTI_CORDINATE;
      }
    } else { // not on map
      off_map.push_back(i.first);
    }
  }
  
  print_off_map(off_map);
}

// print the provided grid, regardless of view type
void MapView::print_grid(const Grid_t &grid)
{
  // dont print decimal points
  auto old_prec = cout.precision();
  cout.precision(0);
  for (int i = size-1; i >= 0; --i) {
    // y axis labels
    if(i % 3 == 0) {
      double label = round(i * scale + origin.y);
      cout << setw(4) << label << " ";
    } else cout << "     ";
    for (auto &str : grid[i]) {
      cout << setw(2) << str;
    }
    cout << endl;
  }
  
  // label x axis
  for (int i = 0; i < size; ++i) {
    if (i % 3 == 0) {
      double label = round(i * scale + origin.x);
      cout << "  " << setw(4) << label;
    }
  }
  cout << endl;
  
  // restore precision
  cout.precision(old_prec);
}

// Calculate the cell subscripts corresponding to the supplied location parameter,
// using the current size, scale, and origin of the display.
// This function assumes that origin is a  member variable of type Point,
// scale is a double value, and size is an integer for the number of rows/columns
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool MapView::get_subscripts(int &ix, int &iy, Point location) const
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
  } else
		return true;
}
