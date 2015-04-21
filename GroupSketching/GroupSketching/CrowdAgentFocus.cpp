#include "stdafx.h"
#include "CrowdAgentFocus.h"
#include <math.h>


CrowdAgentFocus::CrowdAgentFocus(void)
{
}

CrowdAgentFocus::CrowdAgentFocus(Formation* f1, Formation* f2, Path path) 
{
	startFormation = f1;
	endFormation = f2;
	//Get agents from agent coordinates
	vector<glm::vec3> agentCoords = f1->getAgentCoords();
	vector<glm::vec3> endCoords = f2->getAgentCoords();
	for (unsigned int i=0; i<agentCoords.size(); i++) {
		int rndm = rand() % 3;
		cout << rndm;
		glm::vec3 colour;
		if (rndm==1) {
			colour = glm::vec3 (30.0/256.0, 60.0/256.0, 200.0/256.0);
		} else if (rndm==2) {
			colour = glm::vec3 (200.0/256.0, 30.0/256.0, 30.0/256.0);
		} else {
			colour = glm::vec3 (45.0/256.0, 200.0/256.0, 50.0/256.0);
		}
		AgentFocus* agent = new AgentFocus(agentCoords[i], endCoords[i], colour);
		cout << rndm;
		//cout << agentCoords[i].x << agentCoords[i].y << agentCoords[i].z << endl;
		agents.push_back(agent);
	}
}

CrowdAgentFocus::CrowdAgentFocus(Formation* f1, Formation* f2, Path path, vector<AgentFocus*> agents)
{
	startFormation = f1;
	endFormation = f2;
	this->agents = agents;
	
	//Get the list of end coords from the second formation
	vector<glm::vec3> coords = f2->getAgentCoords();

	//Set each agent's destination point to be this point, but relative to their current position
	for (unsigned int i=0; i<agents.size(); i++) {
		agents[i]->setEndPoint(coords[i]);
		agents[i]->setPath(path);
		agents[i]->setColour(vec3(1.0, 0.0, 0.0));
	}
}

//Add more constructors - Formation, Sub-formation, Formation, Sub-formation, Path
//Formation, Sub-formation, Formation, Sub-formation, Path, Sub-path
//Formation, list of sub-formation, Formation, List of sub-formations, Path, list of sub-paths

CrowdAgentFocus::CrowdAgentFocus(Formation* f1, Formation* f1s, Formation* f2, Formation* f2s, Path path, Path subPath, vector<AgentFocus*> agents, vector<AgentFocus*> subAgents)
{
	startFormation = f1;
	endFormation = f2;
	this->agents = agents;
	this->subAgents = subAgents;

	//Get the list of end coords from the second formation
	vector<glm::vec3> coords = f2->getAgentCoords();

	//Set each agent's destination point to be this point
	for (unsigned int i=0; i<agents.size(); i++) {
		agents[i]->setEndPoint(coords[i]);
		agents[i]->setPath(path);
		agents[i]->setColour(vec3(1.0, 0.0, 0.0));
	}

	//Do the same things for the agents in the sub-formation
	vector<vec3> subCoords = f2s->getAgentCoords();

	for(unsigned int i=0; i<subAgents.size(); i++) {
		subAgents[i]->setEndPoint(subCoords[i]);
		subAgents[i]->setPath(subPath);
		subAgents[i]->setColour(vec3(0.0, 0.0, 1.0));
	}
}

CrowdAgentFocus::~CrowdAgentFocus(void)
{
}

void CrowdAgentFocus::update(vector<AgentFocus*> neighbours, vector<AgentFocus*> subNeighbours)
{
	//Insert the list of agents into the neighbour list (this should later be modified on an agent-by-agent basis)
	neighbours.insert(neighbours.begin(), agents.begin(), agents.end());
	for (unsigned int i=0; i<agents.size(); i++) {
		//Give agents the list of their neighbours, as well as the urgency (which is the same for all agents in a single update)
		agents[i]->update(neighbours);
		if (i<agents.size()-1) {
			neighbours[i] = agents[i+1];
		}
	}

	if (subAgents.size() > 0) {
		//Now update the sub-agents
		subNeighbours.insert(subNeighbours.begin(), subAgents.begin(), subAgents.end());
		for (unsigned int i=0; i<subAgents.size(); i++) {
			subAgents[i]->update(subNeighbours);
			if (i<subAgents.size()-1) {
				subNeighbours[i] = subAgents[i+1];
			}
		}
	}
}

vector<vec3> CrowdAgentFocus::getRelativeAgentCoords()
{
	vector<vec3> coords;
	for (unsigned int i=0; i<agents.size(); i++) {
		coords.push_back(agents[i]->getPosition());
	}
	return coords;
}

vector<vec3> CrowdAgentFocus::getAbsoluteAgentCoords()
{
	vector<vec3> coords;
	for (unsigned int i=0; i<agents.size(); i++) {
		coords.push_back(agents[i]->getPosition()+centre);
	}
	return coords;
}