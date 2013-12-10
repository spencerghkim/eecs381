#include "Controller.h"

#include "AgentComponent.h"
#include "AgentGroup.h"
#include "AgentIndividual.h"
#include "Agent_factory.h"
#include "AmountsView.h"
#include "AttackView.h"
#include "FullMapView.h"
#include "Geometry.h"
#include "HealthView.h"
#include "ListView.h"
#include "LocalMapView.h"
#include "MapView.h"
#include "Model.h"
#include "Sim_object.h"
#include "Structure.h"
#include "Structure_factory.h"
#include "Utility.h"
#include "View.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>

using std::bad_alloc;
using std::cout; using std::cin; using std::endl;
using std::string; using std::all_of;
using std::shared_ptr;
using std::make_shared;
using std::static_pointer_cast;
using std::find_if;

// simple helper functions
void bad_command();
int read_int();
double read_double();
Point read_point();
string read_object_name();

// init control function maps
Controller::Controller()
{
  view_mgmt_cmds["open"]    = &Controller::view_open;
  view_mgmt_cmds["close"]   = &Controller::view_close;
  
  map_view_cmds["default"]  = &Controller::view_default;
  map_view_cmds["size"]     = &Controller::view_size;
  map_view_cmds["zoom"]     = &Controller::view_zoom;
  map_view_cmds["pan"]      = &Controller::view_pan;
  
  program_cmds["status"]    = &Controller::prog_status;
  program_cmds["show"]      = &Controller::prog_show;
  program_cmds["go"]        = &Controller::prog_go;
  program_cmds["build"]     = &Controller::prog_build;
  program_cmds["train"]     = &Controller::prog_train;
  
  agent_cmds["move"]        = &Controller::agent_move;
  agent_cmds["work"]        = &Controller::agent_work;
  agent_cmds["attack"]      = &Controller::agent_attack;
  agent_cmds["stop"]        = &Controller::agent_stop;
  
  // add new group commands
  program_cmds["group"]     = &Controller::group_create;
  program_cmds["disband"]   = &Controller::group_disband;
  agent_cmds["add"]         = &Controller::group_add;
  agent_cmds["remove"]      = &Controller::group_remove;

}

// create View object, run the program
void Controller::run()
{
  while (1) {
    try {
      string command;
      cout << "\nTime " << Model::get().get_time() << ": Enter command: ";
      cin >> command;
      
      if (command == "quit") {
        cout << "Done" << endl;
        break;
      }
      
      CmdFunc_t::iterator program_cmd;
      CmdFunc_t::iterator view_mgmt_cmd;
      CmdFunc_Map_View_t::iterator map_view_cmd;
      CmdFunc_t::iterator group_mgmt_cmd;
      
      // Call the correct function based on the command word.
      if (Model::get().is_agent_component_present(command)) {
        shared_ptr<AgentComponent> agent = Model::get().get_agent_comp_ptr(command);
        
        // Check that the agent command is valid.
        cin >> command;
        auto agent_command = agent_cmds.find(command);
        if (agent_command == agent_cmds.end()) {
          throw Error("Unrecognized command!");
        }
        
        agent_command->second(this, agent);
      } else if ((program_cmd = program_cmds.find(command)) != program_cmds.end()) {
        program_cmd->second(this);
      } else if ((view_mgmt_cmd = view_mgmt_cmds.find(command)) != view_mgmt_cmds.end()) {
        view_mgmt_cmd->second(this);
      } else if ((map_view_cmd = map_view_cmds.find(command)) != map_view_cmds.end()) {
        auto map = map_view.lock();
        if (!map) {
          throw Error("No map view is open!");
        }
        map_view_cmd->second(this, map);
      } else {
        throw Error("Unrecognized command!");
      }
      
    } catch (Error &e) {
      cout << e.msg << endl;
      // clear line
      cin.clear();
      while(cin.get() != '\n');
    } catch (bad_alloc &a) {
      cout << "Bad allocation" << endl;
      break;
    } catch (...) {
      cout << "Unknown Exception Caught!" << endl;
      break;
    }
  }
}

// view commands //

void Controller::view_open()
{
  string name;
  cin >> name;
  
  // Check to see if view already exists.
  auto open_view_itr =
      find_if(views.begin(), views.end(), [&] (Views_t::value_type& view) { return view.name == name; });
  if (open_view_itr != views.end()) {
    throw Error("View of that name already open!");
  }
  
  // Create new view and attach to model.
  auto view = create_view(name);
  Model::get().attach(view);
  views.push_back({name, view});
}

void Controller::view_close()
{
  string name;
  cin >> name;
  
  // Check that the view is open.
  auto open_view_itr =
      find_if(views.begin(), views.end(), [&] (Views_t::value_type& view) { return view.name == name; });
  if (open_view_itr == views.end()) {
    throw Error("No view of that name is open!");
  }
  
  // Detach view from Model and destroy it.
  Model::get().detach(open_view_itr->view);
  views.erase(open_view_itr);
}

void Controller::view_default(std::shared_ptr<FullMapView> map)
{
  map->set_defaults();
}
void Controller::view_size(std::shared_ptr<FullMapView> map)
{
  map->set_size(read_int());
}
void Controller::view_zoom(std::shared_ptr<FullMapView> map)
{
  map->set_scale(read_double());
}
void Controller::view_pan(std::shared_ptr<FullMapView> map)
{
  map->set_origin(read_point());
}

// view factory
shared_ptr<View> Controller::create_view(const string& name)
{
  shared_ptr<View> view;
  if (name == "map"){
    auto new_view = make_shared<FullMapView>();
    map_view = new_view;
    view = new_view;
  } else if (name == "health") {
    view = make_shared<HealthView>();
  } else if (name == "amounts") {
    view = make_shared<AmountsView>();
  } else if (name == "attack") {
    view = make_shared<AttackView>();
  } else if (Model::get().object_fullname_exists(name)) {
    view = make_shared<LocalMapView>(name);
  } else {
    throw Error("No object of that name!");
  }
  return view;
}

// whole-program commands //

void Controller::prog_status()
{
  Model::get().describe();
}
void Controller::prog_show()
{
  for (auto &v : views) {
    v.view->draw();
  }
}
void Controller::prog_go()
{
  Model::get().update();
}
void Controller::prog_build()
{
  // read in a valid name and type
  string name = read_object_name();
  string type;
  cin >> type;
  
  // create/add the structure to the model
  shared_ptr<Structure>s = create_structure(name, type, read_point());
  Model::get().add_structure(s);
}

void Controller::prog_train()
{
  // read in a valid name and type
  string name = read_object_name();
  string type;
  cin >> type;
  
  // create/add the agent to the model
  shared_ptr<AgentIndividual> agent = create_agent(name, type, read_point());
  Model::get().add_new_agent(agent);
}

// agent commands //

void Controller::agent_move(shared_ptr<AgentComponent> agent)
{
  agent->move_to(read_point());
}
void Controller::agent_work(shared_ptr<AgentComponent> agent)
{
  string source, destination;
  cin >> source >> destination;
  shared_ptr<Structure> src = Model::get().get_structure_ptr(source);
  shared_ptr<Structure> dest = Model::get().get_structure_ptr(destination);
  agent->start_working(src, dest);
}
void Controller::agent_attack(shared_ptr<AgentComponent> attacker)
{
  string agent_name;
  cin >> agent_name;

  auto target = Model::get().get_agent_comp_ptr(agent_name);  
  attacker->start_attacking(target);
}
void Controller::agent_stop(shared_ptr<AgentComponent> agent)
{
  agent->stop();
}

// group commands //

void Controller::group_create()
{
  string group_name;
  cin >> group_name;
  Model::get().add_new_agent_component(make_shared<AgentGroup>(group_name));
}

void Controller::group_disband()
{
  string group_name;
  cin >> group_name;
  auto group = Model::get().get_agent_comp_ptr(group_name);
  
  // remove all members from the group
  group->disband();
  // remove the group from the model
  Model::get().remove_agent_component(group_name);
}

void Controller::group_add(std::shared_ptr<AgentComponent> group)
{
  string agent_name;
  cin >> agent_name;
  
  // Get the agent component and call Model to handle insertion and what not.
  shared_ptr<AgentComponent> component = Model::get().get_agent_comp_ptr(agent_name);
  Model::get().add_agent_component_to_group(component, group);
}

void Controller::group_remove(std::shared_ptr<AgentComponent> group)
{
  string agent_name;
  cin >> agent_name;
  group->remove_component(agent_name);
}

// HELPERS //

// read int, error for non-digts
int read_int()
{
  int num;
  if (!(cin >> num)) {
    throw Error("Expected an integer!");
  }
  return num;
}

// checks valid double
double read_double()
{
  double num;
  if (!(cin >> num)) {
    throw Error("Expected a double!");
  }
  return num;
}

// read in a point
Point read_point()
{
  double x = read_double();
  double y = read_double();
  return {x,y};
}

// read valid obj name
string read_object_name()
{
  string name;
  cin >> name;
  bool plain = all_of(name.begin(), name.end(), isalnum);
  bool in_use = Model::get().is_name_in_use(name);
  if (name.length() < 2 || !plain || in_use) {
    throw Error("Invalid name for new object!");
  }
  return name;
}

