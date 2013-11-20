#include "Controller.h"

#include "Agent.h"
#include "Agent_factory.h"
#include "Geometry.h"
#include "Model.h"
#include "Sim_object.h"
#include "Structure.h"
#include "Structure_factory.h"
#include "Utility.h"
#include "Views.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <string>

using std::string;
using std::cin; using std::cout; using std::endl;
using std::shared_ptr;
using namespace std::placeholders;

int read_int();
double read_double();
string read_object_name();
Point read_point();
bool is_string_alphanumeric(string word);

Controller::Controller()
{
  agent_fp_map.insert(std::make_pair("move", &Controller::move));
  agent_fp_map.insert(std::make_pair("work", &Controller::work));
  agent_fp_map.insert(std::make_pair("attack", &Controller::attack));
  agent_fp_map.insert(std::make_pair("stop", &Controller::stop));
  
  map_view_fp_map.insert(std::make_pair("default", &Controller::view_reset_defaults));
  map_view_fp_map.insert(std::make_pair("size", &Controller::view_set_size));
  map_view_fp_map.insert(std::make_pair("zoom", &Controller::view_set_zoom));
  map_view_fp_map.insert(std::make_pair("pan", &Controller::view_set_pan));
  
  program_fp_map.insert(std::make_pair("status", &Controller::status));
  program_fp_map.insert(std::make_pair("go", &Controller::update_all_and_go));
  program_fp_map.insert(std::make_pair("build", &Controller::build));
  program_fp_map.insert(std::make_pair("train", &Controller::train));
  program_fp_map.insert(std::make_pair("show", &Controller::show));
  
  view_mgmt_fp_map.insert(std::make_pair("open", &Controller::open_view));
  view_mgmt_fp_map.insert(std::make_pair("close", &Controller::close_view));
}

void Controller::run()
{

  while (true) {
    try {

    cout << "\nTime " << Model::getInstance().get_time() << ": Enter command: ";

    string word;
    cin >> word;
    
    if (word == "quit") {
      // TODO: I don't need to detach views here right?
      cout << "Done" << endl;
      return;
    }
    
    auto program_fp_it = program_fp_map.find(word);
    auto map_view_fp_it = map_view_fp_map.find(word);
    auto view_mgmt_fp_it = view_mgmt_fp_map.find(word);

    if (Model::getInstance().is_agent_present(word)) {
      shared_ptr<Agent> agent = Model::getInstance().get_agent_ptr(word);
      
      if (!agent->is_alive()) {
        throw Error{"Agent is not alive!"};
      }

      string command;
      cin >> command;
      auto it = agent_fp_map.find(command);
      if (it == agent_fp_map.end()) {
        throw Error{"Unrecognized command!"};
      }
      
      // WOW, this is ugly.
      (this->*(it->second))(agent);
    } else if (program_fp_it != program_fp_map.end()) {
      (this->*(program_fp_it->second))();
    } else if (map_view_fp_it != map_view_fp_map.end()) {
      // Check that we actually have a map view open.
      if (!open_map_view_ptr) {
        throw Error{"No map view is open!"};
      }
      
      (this->*(map_view_fp_it->second))(open_map_view_ptr);
    } else if (view_mgmt_fp_it != view_mgmt_fp_map.end()) {
      string view_name;
      cin >> view_name;
      (this->*(view_mgmt_fp_it->second))(view_name);
    } else {
      throw Error{"Unrecognized command!"};
    }
      
    } catch (Error& e) {
      cin.clear();
      cout << e.msg << endl;
      while (cin.get() != '\n');
    }
  }
}

// Agent commands
void Controller::move(shared_ptr<Agent> agent)
{
  agent->move_to(read_point());
}

void Controller::work(shared_ptr<Agent> agent)
{
  string source, destination;
  cin >> source;
  shared_ptr<Structure> source_ptr = Model::getInstance().get_structure_ptr(source);
  cin >> destination;
  shared_ptr<Structure> destination_ptr = Model::getInstance().get_structure_ptr(destination);
  
  agent->start_working(source_ptr, destination_ptr);
}

void Controller::attack(shared_ptr<Agent> agent)
{
  string target_agent;
  cin >> target_agent;
  shared_ptr<Agent> target_agent_ptr = Model::getInstance().get_agent_ptr(target_agent);
  
  agent->start_attacking(target_agent_ptr);
}

void Controller::stop(shared_ptr<Agent> agent)
{
  agent->stop();
}

// Map View commands
void Controller::view_reset_defaults(shared_ptr<MapView> map_view)
{
  map_view->set_defaults();
}

void Controller::view_set_size(shared_ptr<MapView> map_view)
{
  int size = read_int();
  map_view->set_size(size);
}

void Controller::view_set_zoom(shared_ptr<MapView> map_view)
{
  double scale = read_double();
  map_view->set_scale(scale);
}

void Controller::view_set_pan(shared_ptr<MapView> map_view)
{
  map_view->set_origin(read_point());
}

// Whole program commands
void Controller::status()
{
  Model::getInstance().describe();
}

void Controller::update_all_and_go()
{
  Model::getInstance().update();
}

void Controller::build()
{
  string name = read_object_name();
  string type;
  cin >> type;
  Model::getInstance().add_structure(create_structure(name, type, read_point()));
}

void Controller::train()
{
  string name = read_object_name();
  string type;
  cin >> type;
  Model::getInstance().add_agent(create_agent(name, type, read_point()));
}

void Controller::show()
{
  std::for_each(open_views.begin(), open_views.end(), std::bind(&View::draw, _1));
}

// View management
void Controller::open_view(const std::string &name)
{
  // Check to make sure the view doesn't already exist.
  for (int i = 0; i < open_views.size(); ++i) {
    if (open_views[i]->get_name() == name) {
      throw Error{"View of that name already open!"};
    }
  }
  
  // Check to make sure view type is valid. It can be a type of map or a sim_object name.
  shared_ptr<View> view_to_add;
  if (name == "map") {
    open_map_view_ptr.reset(new MapView("map"));
    view_to_add = open_map_view_ptr;
  } else if (name == "health") {
    view_to_add.reset(new HealthView("health"));
  } else if (name == "amounts") {
    view_to_add.reset(new AmountView("amounts"));
  } else if (Model::getInstance().is_name_in_use(name)) {
    view_to_add.reset(new LocalView(name));
  } else {
    throw Error{"No object of that name!"};
  }
  
  // Add the view to the model and to our open views vector.
  Model::getInstance().attach(view_to_add);
  open_views.push_back(view_to_add);
}

void Controller::close_view(const std::string &name)
{
  // Find the view.
  int index = -1;
  for (int i = 0; i < open_views.size(); ++i) {
    if (open_views[i]->get_name() == name) {
      index = i;
    }
  }
  
  if (index == -1) {
    throw Error{"No view of that name is open!"};
  }
  
  // Special check if the map container is going away.
  if (open_views[index]->get_name() == "map") {
    open_map_view_ptr.reset();
  }
  
  // Remove from containers.
  Model::getInstance().detach(open_views[index]);
  open_views.erase(open_views.begin() + index);
}

// Utils
int read_int()
{
  int num;
  if (!(cin >> num)) {
    throw Error{"Expected an integer!"};
  }
  return num;
}

double read_double()
{
  double num;
  if (!(cin >> num)) {
    throw Error{"Expected a double!"};
  }
  return num;
}

Point read_point() {
  double x = read_double();
  double y = read_double();
  return Point(x, y);
}

string read_object_name()
{
  string name;
  cin >> name;
  if (name.length() < 2 ||
      !is_string_alphanumeric(name) ||
      Model::getInstance().is_name_in_use(name)) {
    throw Error{"Invalid name for new object!"};
  }
  return name;
}

bool is_string_alphanumeric(string word)
{
  for (char c : word) {
    if (!isalnum(c)) {
      return false;
    }
  }
  return true;
}
