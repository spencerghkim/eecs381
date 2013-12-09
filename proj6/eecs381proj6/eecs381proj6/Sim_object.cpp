#include "Sim_object.h"

#include "Geometry.h"

#include <iostream>
#include <string>

using std::string;

Sim_object::Sim_object(const string& name_) :
  name{name_} {}

// Explicit default destructor.
Sim_object::~Sim_object() {}
