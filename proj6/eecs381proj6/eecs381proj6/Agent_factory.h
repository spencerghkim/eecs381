#ifndef AGENT_FACTORY_H
#define AGENT_FACTORY_H

#include <memory>
#include <string>

class AgentIndividual;
struct Point;

// Create and return the pointer to the specified AgentComponent type. If the type
// is unrecognized, throws Error("Trying to create agent of unknown type!")
// The AgentComponent is allocated with new, so some other component is resposible for deleting it.
std::shared_ptr<AgentIndividual> create_agent(const std::string& name, const std::string& type, Point location);

#endif
