/* Controller
This class is responsible for controlling the Model and View according to interactions
with the user.
*/

#include <map>
#include <string>

class Agent;
class View;

class Controller {
public:	
	Controller();

	// create View object, run the program by acccepting user commands, then destroy View object
	void run();
  
private:
  using agent_command_fp = void (Controller::*)(Agent *);
  using program_command_fp = void (Controller::*)();
  using view_command_fp = void (Controller::*)(View *);
  
  using Agent_fp_map_c = std::map<std::string, agent_command_fp>;
  using Program_fp_map_c = std::map<std::string, program_command_fp>;
  using View_fp_map_c = std::map<std::string, view_command_fp>;
  
  Agent_fp_map_c agent_fp_map;
  Program_fp_map_c program_fp_map;
  View_fp_map_c view_fp_map;
  
  // View commands.
  void view_reset_defaults(View *);
  void view_set_size(View *);
  void view_set_zoom(View *);
  void view_set_pan(View *);
  void show(View *);
  
  // Whole program commands.
  void status();
  void update_all_and_go();
  void build();
  void train();
  
  // Agent commands.
  void move(Agent* agent);
  void work(Agent* agent);
  void attack(Agent* agent);
  void stop(Agent* agent);
};
