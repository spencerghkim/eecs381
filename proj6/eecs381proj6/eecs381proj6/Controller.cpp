#include "Controller.h"

#include "Agent.h"
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

//BIG TODO: make a separate view factory so we dont have 6 View Includes...

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
shared_ptr<Structure> read_struct();
shared_ptr<Agent> read_agent();
string read_object_name();

// init control function maps
Controller::Controller()
{
  ctrl_view_cmds["open"]    = &Controller::view_open;
  ctrl_view_cmds["close"]   = &Controller::view_close;
  
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
      
      if (Model::get().is_agent_present(command)) {
        shared_ptr<Agent> a = Model::get().get_agent_ptr(command);
        assert(a->is_alive());
        
        cin >> command;
        auto a_itr = agent_cmds.find(command);
        if (a_itr != agent_cmds.end()) {
          a_itr->second(this, a);
        } else bad_command();
        
      } else { // not an agent name
        auto p_itr = program_cmds.find(command);
        if (p_itr != program_cmds.end()) {
          p_itr->second(this);
        } else {
          auto cv_cmd = ctrl_view_cmds.find(command);
          auto mv_cmd = map_view_cmds.find(command);
          if (cv_cmd != ctrl_view_cmds.end()) {
            cv_cmd->second(this);
          } else if(mv_cmd != map_view_cmds.end()) {
            auto map = map_view.lock();
            if(!map) {
              throw Error("No map view is open!");
            }
            mv_cmd->second(this, map);
          } else  bad_command();
        }
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
  auto itr = get_view_itr(name);
  if (itr != views.end()) {
    throw Error("View of that name already open!");
  }
  auto view = create_view(name);
  // attach to the model
  Model::get().attach(view);
  views.push_back({name, view});
}

void Controller::view_close()
{
  string name;
  cin >> name;
  auto itr = get_view_itr(name);
  if (itr == views.end()) {
    throw Error("No view of that name is open!");
  }
  Model::get().detach(itr->view);
  views.erase(itr);
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

// return an iterator to the view, or throw
Controller::Views_t::iterator Controller::get_view_itr(const string &name)
{
  return find_if(views.begin(), views.end(), [&name](Views_t::value_type &v){
    return v.name == name;
  });
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
  // read in a valid name
  string name = read_object_name();
  
  string type;
  cin >> type;
  
  // create/add the structure to the model
  shared_ptr<Structure>s = create_structure(name, type, read_point());
  Model::get().add_structure(s);
}

void Controller::prog_train()
{
  // read in a valid name
  string name = read_object_name();
  
  string type;
  cin >> type;
  
  // create/add the structure to the model
  shared_ptr<Agent>a = create_agent(name, type, read_point());
  Model::get().add_agent(a);
}

// agent commands //

void Controller::agent_move(shared_ptr<Agent> a)
{
  a->move_to(read_point());
}
void Controller::agent_work(shared_ptr<Agent> a)
{
  shared_ptr<Structure>src = read_struct();
  shared_ptr<Structure>dest = read_struct();
  a->start_working(src, dest);
}
void Controller::agent_attack(shared_ptr<Agent> a)
{
  a->start_attacking(read_agent());
}
void Controller::agent_stop(shared_ptr<Agent> a)
{
  a->stop();
}

// HELPERS //

void bad_command()
{
	throw Error("Unrecognized command!");
}

// read int, error for non-digts
int read_int()
{
  int num;
  if(!(cin >> num)) {
    throw Error("Expected an integer!");
  }
  return num;
}

// checks valid double
double read_double()
{
  double num;
  if(!(cin >> num)) {
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

// get ptr to struct in model
shared_ptr<Structure> read_struct()
{
  string name;
  cin >> name;
  return Model::get().get_structure_ptr(name);
}

// get ptr to agent in model
shared_ptr<Agent> read_agent()
{
  string name;
  cin >> name;
  return Model::get().get_agent_ptr(name);
}

// read valid obj name
string read_object_name()
{
  string name;
  cin >> name;
  bool plain = all_of(name.begin(), name.end(), isalnum);
  bool in_use = Model::get().is_name_in_use(name);
  if(name.length() < 2 || !plain || in_use)
    throw Error("Invalid name for new object!");
  return name;
}

