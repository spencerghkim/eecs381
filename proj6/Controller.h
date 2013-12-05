/* Controller
 This class is responsible for controlling the Model and View according to interactions
 with the user.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>

class View;
class FullMapView;
class Agent;

class Controller {
public:
	Controller();
  
	// create View object, run the program by
  // acccepting user commands, then destroy View object
	void run();
  
private:
  //TODO: make sure std::function is Kosher, its much cleaner though
  using CmdFunc_t = std::map<std::string, std::function<void(Controller*)>>;
  using CmdFunc_Agent_t = std::map<std::string, std::function<void(Controller*, std::shared_ptr<Agent>)>>;
  
  using viewPair_t = struct {
    std::string name;
    std::shared_ptr<View> view;
  };
  using Views_t = std::list<viewPair_t>;
  
  // view commands
  void view_open();
  void view_close();
  void view_default();
  void view_size();
  void view_zoom();
  void view_pan();
  
  // return an iterator to the view, or throw
  Views_t::iterator get_view_itr(const std::string &name);
  // return the map view, or throw
  std::shared_ptr<FullMapView> get_map_view();
  // view factory
  std::shared_ptr<View> create_view(const std::string& name);
  
  // whole-program commands
  void prog_status();
  void prog_show();
  void prog_go();
  void prog_build();
  void prog_train();
  
  // agent commands
  void agent_move(std::shared_ptr<Agent>);
  void agent_work(std::shared_ptr<Agent>);
  void agent_attack(std::shared_ptr<Agent>);
  void agent_stop(std::shared_ptr<Agent>);
  
  // containers
  CmdFunc_t program_cmds;
  CmdFunc_Agent_t agent_cmds;
  CmdFunc_t view_cmds;
  Views_t views;
};

#endif