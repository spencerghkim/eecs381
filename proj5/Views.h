#ifndef VIEW_H_
#define VIEW_H_

/* View is an abstract base class that provides a fat interface for child views. */

#include "Geometry.h"

#include <map>
#include <string>
#include <vector>

struct Point;

class View {
public:
  
  View(const std::string& name_) : name{name_} {}
  
  // make class abstract
  virtual ~View() = 0;
	
	// Empty implmentation for fat interface.
	virtual void update_location(const std::string& name, Point location) {}
  
  // Empty implementation for fat interface.
  virtual void update_health(const std::string& name, double health) {}
  
  // Empty implementation for fat interface.
  virtual void update_amount(const std::string& name, double amount) {}
	
	// Remove the named object from the view.
	virtual void update_remove(const std::string& name) = 0;
	
	// Displays the map on the console.
	virtual void draw() = 0;
	
	// Discard the saved information - drawing will show only a empty pattern
	virtual void clear() = 0;
  
  const std::string& get_name() const
      { return name; }
  
private:
  std::string name;
};

class LocationView : public View {
public:
  
  LocationView(const std::string& name_) : View(name_) {}
  
  // Make this class abstract.
  virtual ~LocationView() = 0;
  
  // Override to keep track of objects' locations.
  void update_location(const std::string& name, Point location) override;
  
  // Remove a single object from the view.
  void update_remove(const std::string& name) override;
  
  // Draw the view using a grid layout.
  void draw() override;
  
  // Remove all objects.
  void clear() override;
  
protected:
  
  // Print the names of the objects that are offscreen.
  void print_objects_offscreen() const;
  
  // Print the map scale, size, origin
  void print_metrics() const;
  
  // Provide setters for subclasses.
  
	// if the size is out of bounds will throw Error("New map size is too big!")
	// or Error("New map size is too small!")
	void set_size(int size_);
  
	// If scale is not postive, will throw Error("New map scale must be positive!");
	void set_scale(double scale_);
	
	// any values are legal for the origin
	void set_origin(Point origin_);
  
  // set the center of the view to the given point
  void set_center(Point center);
  
private:
  
  Point origin;
  double scale;
  int size;  // Number of rows/columns in the grid
  std::map<std::string, Point> locations;
  
  bool get_subscripts(int &ix, int &iy, Point location) const;
};

class MapView : public LocationView {
public:
  
  // Set up scale and such in constructor.
  MapView(const std::string& name_);
  
  void draw() override;
  
  // Expose these setters publicly.
  void set_size(int size_)
      { LocationView::set_size(size_); }
  
  void set_scale(double scale_)
      { LocationView::set_scale(scale_); }
  
  void set_origin(Point origin_)
      { LocationView::set_origin(origin_); }
  
  void set_defaults();
};

class LocalView : public LocationView {
public:
  
  LocalView(const std::string& name_);
  
  // Override the update_location method to update the origin.
  void update_location(const std::string& name, Point location) override;
  
  void draw() override;
};

class AttributeView : public View {
public:
  
  AttributeView(const std::string name_) : View(name_) {}
  
  virtual ~AttributeView() = 0;
  
  // Remove a single object from the attributes map.
  void update_remove(const std::string& name) override;
  
  void draw() override;
  
  void clear() override;
  
protected:
  // Let subclasses add/alter an attribute.
  void set_attribute(const std::string& name, double attribute);
  
private:
  
  std::map<std::string, double> attributes;
  
  // Subclasses must return the type of attribute.
  virtual std::string get_attribute_name() = 0;
};

class HealthView : public AttributeView {
public:
  
  HealthView(const std::string& name_) : AttributeView(name_) {}
  
  void update_health(const std::string& name, double health) override;
    
  std::string get_attribute_name() override;
};

class AmountView : public AttributeView {
public:
  
  AmountView(const std::string& name_) : AttributeView(name_) {}
    
  void update_amount(const std::string& name, double amount) override;
  
  std::string get_attribute_name() override;
};

#endif
