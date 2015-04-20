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
	this->path = path;
	urgency = 0.1f;
	rotation = 0;
	//Get agents from agent coordinates
	vector<glm::vec3> agentCoords = f1->getAgentCoords();
	vector<glm::vec3> endCoords = f2->getAgentCoords();
	pathLength = 0;
	for (unsigned int i=0; i<path.size()-1; i++) {
		pathLength += length(path[i+1]-path[i]);
	}
	pathLeft = pathLength;
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
	this->path = path;
	this->agents = agents;
	rotation = 0;
	urgency = 0.1f;
	//Set the current position, and the first destination (and remove this from the vector)
	currentPosition = f1->getCentre();
	
	pathLength = length(path[0]-currentPosition);
	for (unsigned int i=0; i<path.size()-1; i++) {
		pathLength += length(path[i+1]-path[i]);
	}
	pathLeft = pathLength;

	currentDestination = path.front();
	path.erase(path.begin());
	
	//Get the list of end coords from the second formation
	vector<glm::vec3> coords = f2->getAgentCoords();

	//Set each agent's destination point to be this point, but relative to their current position
	for (unsigned int i=0; i<agents.size(); i++) {
		agents[i]->setEndPoint(coords[i]-f2->getCentre());
	}

	//Set the crowd's pathVec to point towards the current destination.
	pathVec = normalize(currentDestination-currentPosition)*0.1f;
}

//Add more constructors - Formation, Sub-formation, Formation, Sub-formation, Path
//Formation, Sub-formation, Formation, Sub-formation, Path, Sub-path
//Formation, list of sub-formation, Formation, List of sub-formations, Path, list of sub-paths

CrowdAgentFocus::CrowdAgentFocus(Formation* f1, Formation* f1s, Formation* f2, Formation* f2s, Path path, Path subPath, vector<AgentFocus*> agents, vector<AgentFocus*> subAgents)
{
	startFormation = f1;
	endFormation = f2;
	this->path = path;
	this->agents = agents;
	this->subAgents = subAgents;
	rotation = 0;
	urgency = 0.1f;
	//Set the current position, and the first destination (and remove this from the vector)
	currentPosition = f1->getCentre();
	
	pathLength = length(path[0]-currentPosition);
	for (unsigned int i=0; i<path.size()-1; i++) {
		pathLength += length(path[i+1]-path[i]);
	}
	pathLeft = pathLength;

	
	
	//Get the list of end coords from the second formation
	vector<glm::vec3> coords = f2->getAgentCoords();

	//Set each agent's destination point to be this point
	for (unsigned int i=0; i<agents.size(); i++) {
		agents[i]->setEndPoint(coords[i]);
		agents[i]->setPath(path);
	}


	//Do the same things for the agents in the sub-formation
	vector<vec3> subCoords = f2s->getAgentCoords();

	for(unsigned int i=0; i<subAgents.size(); i++) {
		subAgents[i]->setEndPoint(subCoords[i]);

	}

	//Set the crowd's pathVec to point towards the current destination.
	pathVec = normalize(currentDestination-currentPosition)*0.1f;
}

CrowdAgentFocus::~CrowdAgentFocus(void)
{
}

void CrowdAgentFocus::update(vector<AgentFocus*> neighbours)
{
	//v1 (done): Just update each agent individually, don't work out neighbours
	//v2 (done): All agents in the crowd are neighbours, none outside are
	//v3 (done): Agents are also given the crowd vector, which follows the path. This is calculated each frame.
	//v4 (done): Neighbours are all agents in current crowd within a threshold
	//v5 (future): Neighbours are all agents in current and nearby crowds within a threshold
	//v6 (future, potentially slower and less useful): Neighbours are only drawn from a number of 5x5 blocks around the current agent
	
	//Insert the list of agents into the neighbour list (this should later be modified on an agent-by-agent basis)
	neighbours.insert(neighbours.begin(), agents.begin(), agents.end());

	//Calculate the crowd vector:
	//First, check if it is close to the desired point.
	currentPosition += pathVec;
	if (length(currentDestination-currentPosition)<0.1) {
		//If so, check if any points exist.
		if (path.size()>0) {
			//If so, pop the first one and set that as the destination, after reducing the total distance left to travel.
			pathLeft -= length(currentDestination-currentPosition);
			currentDestination = path.front();
			path.erase(path.begin());
			
			//Work out the vector to travel in, which is in that direction but a much smaller, static, speed.
			vec3 heading = normalize(currentDestination-currentPosition)*0.1f;
			pathVec = heading;
		} else {
			//Otherwise, the agents are at their destination, so set the path vector to 0.
			pathVec = vec3(0, 0, 0);
		}
	} else {
		//If the point is not close enough, the current vector is alright so keep heading in this direction, reducing the distance left to travel.
		pathLeft -= length(pathVec);
	}

	//Need to check how far the main and sub-formations are along their paths, and adjust their speeds accordingly.




	//Recalculate the urgency
	//pathLeft/pathLength varies from 1 to 0, and the urgency from 0.1 to 3.1.
	//Enter the following in Wolfram Alpha to see the urgency values for a pathLength of 100 (in which case the multiplier, k, is 10)
	//y=3e^(-10x)+0.1, x = 0 to 1, y = 0 to 3.5
	//If the pathLength = 400, k = 40, so the agents will only begin move with urgency much later along the path.

	float k = (pathLength*(10.0f/100.0f));
	float x = (float) pathLeft/pathLength;
	urgency = (3*exp(-k*x))+0.1f;
	//Append agents to beginning of neighbours
	//Each iteration in this loop, replace i (or i-1?) with i+1 (or i?)
	/*
	?(0, 1) = pi/2
	?(1, 0) = pi
	?(0, -1) = 3pi/2*/
	glPushMatrix();
		//glTranslated(currentPosition.x, 0, currentPosition.z);
		/*double pathRotation = atan2(pathVec.z, pathVec.x)*180/M_PI;
		//If there is less than a degree between the current rotation and pathRotation, set rotation equal to pathRotation
		if (abs(rotation-pathRotation)<1) {
			rotation = pathRotation;
		} else {
			//Otherwise, change rotation by one degree towards pathRotation.
			rotation < pathRotation ? rotation++ : rotation--;
		}*/

		glRotated(rotation, 0, -1, 0);
		glutSolidCone(1.4, 4.4, 20, 20);
		for (unsigned int i=0; i<agents.size(); i++) {
			//Give agents the list of their neighbours, as well as the urgency (which is the same for all agents in a single update)
			agents[i]->update(neighbours, urgency, currentDestination);
			if (i<agents.size()-1) {
				neighbours[i] = agents[i+1];
			}
		}
		//Now update the sub-agents
		for (unsigned int i=0; i<subAgents.size(); i++) {
			subAgents[i]->update(neighbours, urgency, currentDestination);
		}
	glPopMatrix();
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