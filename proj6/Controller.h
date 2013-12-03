/* Controller
This class is responsible for controlling the Model and View according to interactions
with the user.
*/

#include <map>
#include <memory>
#include <string>
#include <vector>

class Agent;
class View;
class MapView;

class Controller {
public:	
	Controller();

	// create View object, run the program by acccepting user commands, then destroy View object
	void run();
  
private:
  using agent_command_fp = void (Controller::*)(std::shared_ptr<Agent>);
  using map_view_command_fp = void (Controller::*)(std::shared_ptr<MapView>);
  using program_command_fp = void (Controller::*)();
  using view_mgmt_command_fp = void (Controller::*)(const std::string&);
  
  std::map<std::string, agent_command_fp> agent_fp_map;
  std::map<std::string, map_view_command_fp> map_view_fp_map;
  std::map<std::string, program_command_fp> program_fp_map;
  std::map<std::string, view_mgmt_command_fp> view_mgmt_fp_map;
  
  // Vector of open views. The order of Views dictates the order in which they were opened.
  std::vector<std::shared_ptr<View>> open_views;
  
  // There can only be one map view. By keeping track of it, we can easily find out if
  // it has been opened or not, and it prevents us from having to downcast a View to a
  // Map View to call defaults, scale, etc...
  std::shared_ptr<MapView> open_map_view_ptr;
  
  // Map View commands.
  void view_reset_defaults(std::shared_ptr<MapView>);
  void view_set_size(std::shared_ptr<MapView>);
  void view_set_zoom(std::shared_ptr<MapView>);
  void view_set_pan(std::shared_ptr<MapView>);
  
  // View Management Commands.
  void open_view(const std::string& name);
  void close_view(const std::string& name);
  
  // Program commands.
  void status();
  void update_all_and_go();
  void build();
  void train();
  void show();
  
  // Agent commands.
  void move(std::shared_ptr<Agent> agent);
  void work(std::shared_ptr<Agent> agent);
  void attack(std::shared_ptr<Agent> agent);
  void stop(std::shared_ptr<Agent> agent);
};
