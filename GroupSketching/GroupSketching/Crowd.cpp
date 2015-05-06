#include "stdafx.h"
#include "Crowd.h"
#include <math.h>


Crowd::Crowd(void)
{
}

Crowd::Crowd(Formation* f1, Formation* f2, Path path) 
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
		glm::vec3 colour;
		if (rndm==1) {
			colour = glm::vec3 (30.0/256.0, 60.0/256.0, 200.0/256.0);
		} else if (rndm==2) {
			colour = glm::vec3 (200.0/256.0, 30.0/256.0, 30.0/256.0);
		} else {
			colour = glm::vec3 (45.0/256.0, 200.0/256.0, 50.0/256.0);
		}
		Agent* agent = new Agent(agentCoords[i], endCoords[i], colour);

		agents.push_back(agent);
	}
}

Crowd::Crowd(Formation* f1, Formation* f2, Path path, vector<Agent*> agents)
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

Crowd::Crowd(Formation* f1, Formation* f1s, Formation* f2, Formation* f2s, Path path, Path subPath, vector<Agent*> agents, vector<Agent*> subAgents)
{
	startFormation = f1;
	endFormation = f2;
	startSubFormation = f1s;
	endSubFormation = f2s;
	this->path = path;
	this->subPath = subPath;
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

	currentDestination = path.front();
	path.erase(path.begin());
	
	currentSubPosition = f1s->getCentre();

	subPathLength = length(subPath[0]-currentSubPosition);
	for (unsigned int i=0; i<subPath.size()-1; i++) {
		subPathLength += length(subPath[i+1]-subPath[i]);
	}
	subPathLeft = subPathLength;

	currentSubDestination = subPath.front();
	subPath.erase(subPath.begin());

	//Get the list of end coords from the second formation
	vector<glm::vec3> coords = f2->getAgentCoords();

	//Set each agent's destination point to be this point, but relative to their current position
	for (unsigned int i=0; i<agents.size(); i++) {
		agents[i]->setEndPoint(coords[i]-f2->getCentre());
	}

	//Do the same things for the agents in the sub-formation
	vector<vec3> subCoords = f2s->getAgentCoords();

	for(unsigned int i=0; i<subAgents.size(); i++) {
		subAgents[i]->setEndPoint(subCoords[i]-f2s->getCentre());
	}

	//Set the crowd's pathVec to point towards the current destination.
	pathVec = normalize(currentDestination-currentPosition)*0.1f;

	//Set the subPathVec as well.
	subPathVec = normalize(currentSubDestination-currentSubPosition)*0.1f;
}

Crowd::~Crowd(void)
{
}

void Crowd::update(vector<Agent*> neighbours)
{
	//v1 (done): Just update each agent individually, don't work out neighbours
	//v2 (done): All agents in the crowd are neighbours, none outside are
	//v3 (done): Agents are also given the crowd vector, which follows the path. This is calculated each frame.
	//v4 (done): Neighbours are all agents in current crowd within a threshold
	//v5 (future): Neighbours are all agents in current and nearby crowds within a threshold
	//v6 (future, potentially slower and less useful): Neighbours are only drawn from a number of 5x5 blocks around the current agent
	
	//Insert the list of agents into the neighbour list (this should later be modified on an agent-by-agent basis)
	neighbours.insert(neighbours.begin(), agents.begin(), agents.end());

	//Also insert all sub-agents
	neighbours.insert(neighbours.begin(), subAgents.begin(), subAgents.end());

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

	//Do this same thing for the sub-path.
	currentSubPosition += subPathVec;
	if (length(currentSubDestination-currentSubPosition)<0.1) {
		if (subPath.size()>0) {
			subPathLeft -= length(currentSubDestination-currentSubPosition);
			currentSubDestination = subPath.front();
			subPath.erase(subPath.begin());

			vec3 subHeading = normalize(currentSubDestination-currentSubPosition)*0.1f;
			subPathVec = subHeading;
		} else {
			subPathVec = vec3(0, 0, 0);
		}
	} else {
		subPathLeft -= length(subPathVec);
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
	glPushMatrix();
		glTranslated(currentPosition.x, 0, currentPosition.z);
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
			agents[i]->update(neighbours, urgency, pathVec);
			if (i<agents.size()-1) {
				neighbours[i] = agents[i+1];
			}
		}
	glPopMatrix();
	glPushMatrix();
		glTranslated(currentSubPosition.x, 0, currentSubPosition.z);
		//Now update the sub-agents
		//cout << subAgents.size() << ", " << subPathVec.x << ", " << subPathVec.z << ", " << pathVec.x << ", " << pathVec.z << endl;
		for (unsigned int i=0; i<subAgents.size(); i++) {
			subAgents[i]->update(neighbours, urgency, subPathVec);
		}
	glPopMatrix();
	glPushMatrix();
		glColor3f(0.7f, 0.7f, 0.2f);
		for (Agent* agent : agents) {
			vec3 end = agent->getEndPoint();
			glPushMatrix();
				glTranslated(end.x+endFormation->getCentre().x, 0.1, end.z+endFormation->getCentre().z);
				glutSolidSphere(0.3, 25, 25);
			glPopMatrix();
		}
		glColor3f(0.7f, 0.2f, 0.7f);
		for (Agent* agent : subAgents) {
			vec3 end = agent->getEndPoint();
			glPushMatrix();
				glTranslated(end.x+endSubFormation->getCentre().x, 0.1, end.z+endSubFormation->getCentre().z);
				glutSolidSphere(0.3, 25, 25);
			glPopMatrix();
		}
	glPopMatrix();
}

vector<vec3> Crowd::getRelativeAgentCoords()
{
	vector<vec3> coords;
	for (unsigned int i=0; i<agents.size(); i++) {
		coords.push_back(agents[i]->getPosition());
	}
	return coords;
}

vector<vec3> Crowd::getAbsoluteAgentCoords()
{
	vector<vec3> coords;
	for (unsigned int i=0; i<agents.size(); i++) {
		coords.push_back(agents[i]->getPosition()+centre);
	}
	return coords;
}