#ifndef CONTROLLER_H_
#define CONTROLLER_H_

/* Controller
 This class is responsible for controlling the Model and View according to interactions
 with the user.
 */

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>

class View;
class FullMapView;
class AgentComponent;

class Controller {
public:
	Controller();
  
	// create View object, run the program by
  // acccepting user commands, then destroy View object
	void run();
  
private:
  using CmdFunc_t = std::map<std::string, std::function<void(Controller*)>>;
  using CmdFunc_Agent_t = std::map<std::string, std::function<void(Controller*, std::shared_ptr<AgentComponent>)>>;
  using CmdFunc_Map_View_t = std::map<std::string, std::function<void(Controller*, std::shared_ptr<FullMapView>)>>;
  
  using viewPair_t = struct {
    std::string name;
    std::shared_ptr<View> view;
  };
  using Views_t = std::list<viewPair_t>;
  
  // control view commands
  void view_open();
  void view_close();
  
  // map view command
  void view_default(std::shared_ptr<FullMapView> map);
  void view_size(std::shared_ptr<FullMapView> map);
  void view_zoom(std::shared_ptr<FullMapView> map);
  void view_pan(std::shared_ptr<FullMapView> map);
  
  // view factory
  std::shared_ptr<View> create_view(const std::string& name);
  
  // whole-program commands
  void prog_status();
  void prog_show();
  void prog_go();
  void prog_build();
  void prog_train();
  
  // agent commands
  void agent_move(std::shared_ptr<AgentComponent>);
  void agent_work(std::shared_ptr<AgentComponent>);
  void agent_attack(std::shared_ptr<AgentComponent>);
  void agent_stop(std::shared_ptr<AgentComponent>);
  
  // group commands
  void group_create();
  void group_add(std::shared_ptr<AgentComponent> group);
  void group_remove(std::shared_ptr<AgentComponent> group);
  void group_disband(std::shared_ptr<AgentComponent> group);
  
  // weak pointer to prevent 
  std::weak_ptr<FullMapView> map_view;
  
  // containers
  CmdFunc_t program_cmds;
  CmdFunc_Agent_t agent_cmds;
  CmdFunc_t view_mgmt_cmds;
  CmdFunc_Map_View_t map_view_cmds;
  Views_t views;
};

#endif