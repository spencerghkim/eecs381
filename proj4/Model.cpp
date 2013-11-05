#include "Model.h"

#include "Agent_factory.h"
#include "Geometry.h"
#include "Structure_factory.h"

#include <map>

using std::map;

/* Here define g_Model_ptr */
Model* g_Model_ptr = nullptr;

/* *** create the initial structures and agents using the following calls to the factories
in the Model constructor. You will have to write code to save the returned pointers. 
Do not change the order of these calls. You should delete this comment. */
Model::Model() {
	structures.insert(std::make_pair("Rivendale", create_structure("Rivendale", "Farm", Point(10., 10.))));
	structures.insert(std::make_pair("Sunnybrook", create_structure("Sunnybrook", "Farm", Point(0., 30.))));
	structures.insert(std::make_pair("Shire", create_structure("Shire", "Town_Hall", Point(20., 20.))));
	structures.insert(std::make_pair("Paduca", create_structure("Paduca", "Town_Hall", Point(30., 30.))));
	
	agents.insert(std::make_pair("Pippin", create_agent("Pippin", "Peasant", Point(5., 10.))));
  agents.insert(std::make_pair("Merry", create_agent("Merry", "Peasant", Point(0., 25.))));
	agents.insert(std::make_pair("Zug", create_agent("Zug", "Soldier", Point(20., 30.))));
	agents.insert(std::make_pair("Bug", create_agent("Bug", "Soldier", Point(15., 20.))));
}

