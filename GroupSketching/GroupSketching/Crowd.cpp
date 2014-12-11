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
	//Get agents from agent coordinates
	vector<glm::vec3> agentCoords = f1->getAgentCoords();
	vector<glm::vec3> endCoords = f2->getAgentCoords();
	for (int i=0; i<agentCoords.size(); i++) {
		Agent* agent = new Agent(agentCoords[i], endCoords[i]);
		cout << agentCoords[i].x << agentCoords[i].y << agentCoords[i].z << endl;
		agents.push_back(agent);
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

void Crowd::update(vector<glm::vec3> neighbours)
{
	//v1 (done): Just update each agent individually, don't work out neighbours
	//v2 (in-dev): All agents in the crowd are neighbours, none outside are
	//v3 (future): Neighbours are all agents in current crowd within a threshold
	//v4 (future): Neighbours are all agents in current and nearby crowds within a threshold
	//v5 (future, potentially slower and less useful): Neighbours are all agents within a number of 5x5 blocks around the current agent
	for (int i=0; i<agents.size(); i++) {
		otherAgents = agents.
		agents[i]->update();
	}
}