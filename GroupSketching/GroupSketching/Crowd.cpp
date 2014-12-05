#include "stdafx.h"
#include "Crowd.h"


Crowd::Crowd(void)
{
}

Crowd::Crowd(Formation* f1, Formation* f2, Path path) 
{
	startFormation = f1;
	endFormation = f2;
	this->path = path;
	//Default: 50 agents
	//Extended: choose number of agents intelligently
	f1->populate(50);
	//Get agents from agent coordinates
	vector<glm::vec3> agentCoords = f1->getAgentCoords();
	for (int i=0; i<agentCoords.size(); i++) {
		Agent agent (agentCoords[i]);
		agents.push_back(&agent);
	}
}

Crowd::Crowd(Formation* f1, Formation* f2, Path path, vector<Agent*> agents)
{
	startFormation = f1;
	endFormation = f2;
	this->path = path;
	this->agents = agents;
}
Crowd::~Crowd(void)
{
}

void Crowd::update(vector<glm::vec3> endPoints)
{
	//Default: Just update each agent individually, don't work out neighbours
	//Extended: Work out who the neighbours are, pass them to the agents
	for (int i=0; i<agents.size(); i++) {
		vector<Agent*> neighbours;
		agents[i]->update(neighbours, endPoints[i]);
	}
}