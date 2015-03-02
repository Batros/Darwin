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
		float rndm = rand() % 2;
		cout << rndm;
		glm::vec3 colour;
		if (rndm==1) {
			colour = glm::vec3 (30.0/256.0, 60.0/256.0, 200.0/256.0);
		} else {
			colour = glm::vec3 (200.0/256.0, 30.0/256.0, 30.0/256.0);
		}
		Agent* agent = new Agent(agentCoords[i], endCoords[i], colour);
		cout << rndm;
		//cout << agentCoords[i].x << agentCoords[i].y << agentCoords[i].z << endl;
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
	//v5 (future, potentially slower and less useful): Neighbours are only drawn from a number of 5x5 blocks around the current agent
	vector<glm::vec3> relCoords = getRelativeAgentCoords();
	if (neighbours.size()>0) {
		neighbours.insert(neighbours.begin(), relCoords.begin(), relCoords.end());
		neighbours.erase (neighbours.begin());
	}
	else {
		neighbours = relCoords;
	}
	//Append agents to beginning of neighbours
	//Each iteration in this loop, replace i (or i-1?) with i+1 (or i?)
	for (int i=0; i<agents.size(); i++) {

		agents[i]->update(neighbours);
		if (i<agents.size()-1) {
			neighbours[i] = agents[i+1]->getPosition();
		}
	}
}

vector<glm::vec3> Crowd::getRelativeAgentCoords()
{
	vector<glm::vec3> coords;
	for (int i=0; i<agents.size(); i++) {
		coords.push_back(agents[i]->getPosition());
	}
	return coords;
}

vector<glm::vec3> Crowd::getAbsoluteAgentCoords()
{
	vector<glm::vec3> coords;
	for (int i=0; i<agents.size(); i++) {
		coords.push_back(agents[i]->getPosition()+centre);
	}
	return coords;
}