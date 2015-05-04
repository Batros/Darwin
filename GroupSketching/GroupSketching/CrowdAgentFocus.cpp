#include "stdafx.h"
#include "CrowdAgentFocus.h"
#include <math.h>


CrowdAgentFocus::CrowdAgentFocus(void)
{
}

CrowdAgentFocus::CrowdAgentFocus(Formation* f1, Formation* f2, Path path) {
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

	float minSpeed = 100.0f;
	for (unsigned int i=0; i<agents.size(); i++) {
		agents[i]->setEndPoint(coords[i]);
		agents[i]->setPath(path);
		agents[i]->setColour(vec3(1.0, 0.0, 0.0));
		float speed = agents[i]->getSpeed();
		if (speed < minSpeed) {
			minSpeed = speed;
		}
	}

	for (unsigned int i=0; i<agents.size(); i++) {
		agents[i]->setSpeedLimit(minSpeed);
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

	float pathLength = 0.0f;
	if (path.size() > 1) {
		for (int i=1; i<path.size(); i++) {
			pathLength += length(path[i-1] - path[i]);
		}
	}
	else {
		pathLength = length(f1->getCentre() - f2->getCentre());
	}

	float subPathLength = 0.0f;
	if (subPath.size() > 1) {
		for (int i=1; i<subPath.size(); i++) {
			subPathLength += length(subPath[i-1] - subPath[i]);
		}
	}
	else {
		subPathLength = length(f1s->getCentre() - f2s->getCentre());
	}

	//Get the list of end coords from the second formation
	vector<glm::vec3> coords = f2->getAgentCoords();

	//Set each agent's destination point to be this point
	float minSpeed = 100.0f;
	for (unsigned int i=0; i<agents.size(); i++) {
		agents[i]->setEndPoint(coords[i]);
		agents[i]->setPath(path);
		agents[i]->setColour(vec3(1.0, 0.0, 0.0));
		float speed = agents[i]->getSpeed();
		if (speed < minSpeed) {
			minSpeed = speed;
		}
	}

	//Do the same things for the agents in the sub-formation
	vector<vec3> subCoords = f2s->getAgentCoords();

	float minSubSpeed = 100.0f;
	for(unsigned int i=0; i<subAgents.size(); i++) {
		subAgents[i]->setEndPoint(subCoords[i]);
		subAgents[i]->setPath(subPath);
		subAgents[i]->setColour(vec3(0.0, 0.0, 1.0));
		float speed = subAgents[i]->getSpeed();
		if (speed < minSubSpeed) {
			minSubSpeed = speed;
		}
	}

	float travelTime = pathLength / minSpeed;
	float subTravelTime = subPathLength / minSubSpeed;
	if (travelTime < subTravelTime) {
		minSpeed *= travelTime / subTravelTime;
	}
	else {
		minSubSpeed *= subTravelTime / travelTime;
	}

	cout << pathLength << " : " << subPathLength << endl;

	for (unsigned int i=0; i<agents.size(); i++) {
		agents[i]->setSpeedLimit(minSpeed);
	}

	for (unsigned int i=0; i<subAgents.size(); i++) {
		subAgents[i]->setSpeedLimit(minSubSpeed);
	}
}

CrowdAgentFocus::~CrowdAgentFocus(void)
{
}

void CrowdAgentFocus::update(vector<AgentFocus*> neighbours, float sepMod)
{
	//Insert the list of agents into the neighbour list (this should later be modified on an agent-by-agent basis)
	neighbours.insert(neighbours.begin(), agents.begin(), agents.end());
	neighbours.insert(neighbours.begin(), subAgents.begin(), subAgents.end());

	vector<int> agentsToDelete;
	for (unsigned int i=0; i<agents.size(); i++) {
		//Give agents the list of their neighbours, as well as the urgency (which is the same for all agents in a single update)
		agents[i]->update(neighbours, sepMod);
		if (!agents[i]->isStillMoving()) {
			stoppedAgents.push_back(agents[i]);
			agentsToDelete.push_back(i);
		}
	}

	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		agents.erase(agents.begin()+agentsToDelete[i]);
	}

	agentsToDelete.clear();
	if (subAgents.size() > 0) {
		//Now update the sub-agents
		for (unsigned int i=0; i<subAgents.size(); i++) {
			subAgents[i]->update(neighbours, sepMod);
			if (!subAgents[i]->isStillMoving()) {
				stoppedAgents.push_back(subAgents[i]);
				agentsToDelete.push_back(i);
			}
		}
	}

	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		subAgents.erase(subAgents.begin()+agentsToDelete[i]);
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

vector<AgentFocus*> CrowdAgentFocus::getAgents() {
	return agents;
}

void CrowdAgentFocus::removeAgents(vector<int> agentsToDelete) {
	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		agents.erase(agents.begin()+agentsToDelete[i]);
	}
}

vector<AgentFocus*> CrowdAgentFocus::getStoppedAgents() {
	return stoppedAgents;
}

void CrowdAgentFocus::emptyStoppedAgents() {
	stoppedAgents.clear();
}