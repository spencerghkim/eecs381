#include "Controller.h"

#include "Agent.h"
#include "Agent_factory.h"
#include "Model.h"
#include "Structure_factory.h"
#include "Utility.h"
#include "View.h"

#include <iostream>
#include <map>
#include <string>

using std::string;
using std::cin; using std::cout; using std::endl;

int read_int();
double read_double();
string read_object_name();
Point read_point();
bool is_string_alphanumeric(string word);

Controller::Controller()
{
  view_fp_map.insert(std::make_pair("default", &Controller::view_reset_defaults));
  view_fp_map.insert(std::make_pair("size", &Controller::view_set_size));
  view_fp_map.insert(std::make_pair("zoom", &Controller::view_set_zoom));
  view_fp_map.insert(std::make_pair("pan", &Controller::view_set_pan));
  view_fp_map.insert(std::make_pair("show", &Controller::show));
  
  program_fp_map.insert(std::make_pair("status", &Controller::status));
  program_fp_map.insert(std::make_pair("go", &Controller::update_all_and_go));
  program_fp_map.insert(std::make_pair("build", &Controller::build));
  program_fp_map.insert(std::make_pair("train", &Controller::train));
  
  agent_fp_map.insert(std::make_pair("move", &Controller::move));
  agent_fp_map.insert(std::make_pair("work", &Controller::work));
  agent_fp_map.insert(std::make_pair("attack", &Controller::attack));
  agent_fp_map.insert(std::make_pair("stop", &Controller::stop));
}

void Controller::run()
{
  View* view = new View;
  g_Model_ptr->attach(view);
  
  while (true) {
    try {

    cout << "\nTime " << g_Model_ptr->get_time() << ": Enter command: ";

    string word;
    cin >> word;
    
    if (word == "quit") {
      g_Model_ptr->detach(view);
      delete view;
      cout << "Done" << endl;
      return;
    }
    
    auto program_fp_it = program_fp_map.find(word);
    auto view_fp_it = view_fp_map.find(word);

    if (g_Model_ptr->is_agent_present(word)) {
      Agent* agent = g_Model_ptr->get_agent_ptr(word);
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
      // Ugly again. Call it.
      (this->*(program_fp_it->second))();
    } else if (view_fp_it != view_fp_map.end()) {
      // And the third one.
      (this->*(view_fp_it->second))(view);
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

// View commands.
void Controller::view_reset_defaults(View* view)
{
  view->set_defaults();
}

void Controller::view_set_size(View* view)
{
  int size = read_int();
  view->set_size(size);
}

void Controller::view_set_zoom(View* view)
{
  double scale = read_double();
  view->set_scale(scale);
}

void Controller::view_set_pan(View* view)
{
  view->set_origin(read_point());
}

void Controller::show(View* view)
{
  view->draw();
}

// Whole program commands.
void Controller::status()
{
  g_Model_ptr->describe();
}

void Controller::update_all_and_go()
{
  g_Model_ptr->update();
}

void Controller::build()
{
  string name = read_object_name();
  string type;
  cin >> type;
  g_Model_ptr->add_structure(create_structure(name, type, read_point()));
}

void Controller::train()
{
  string name = read_object_name();
  string type;
  cin >> type;
  g_Model_ptr->add_agent(create_agent(name, type, read_point()));
}

// Agent commands.
void Controller::move(Agent* agent)
{
  agent->move_to(read_point());
}

void Controller::work(Agent* agent)
{
  string source, destination;
  cin >> source;
  Structure* source_ptr = g_Model_ptr->get_structure_ptr(source);
  cin >> destination;
  Structure* destination_ptr = g_Model_ptr->get_structure_ptr(destination);
  
  agent->start_working(source_ptr, destination_ptr);
}

void Controller::attack(Agent* agent)
{
  string target_agent;
  cin >> target_agent;
  Agent* target_agent_ptr = g_Model_ptr->get_agent_ptr(target_agent);
  
  agent->start_attacking(target_agent_ptr);
}

void Controller::stop(Agent* agent)
{
  agent->stop();
}

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
      g_Model_ptr->is_name_in_use(name)) {
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
