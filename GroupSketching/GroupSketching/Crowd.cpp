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
		float rndm = rand() % 3;
		cout << rndm;
		glm::vec3 colour;
		if (rndm==1) {
			colour = glm::vec3 (30.0/256.0, 60.0/256.0, 200.0/256.0);
		} else if (rndm==2) {
			colour = glm::vec3 (200.0/256.0, 30.0/256.0, 30.0/256.0);
		} else {
			colour = glm::vec3 (45.0/256.0, 200.0/256.0, 50.0/256.0);
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
	//Set the current position, and the first destination (and remove this from the vector)
	currentPosition = f1->getCentre();
	currentDestination = path.front();
	path.erase(path.begin());

	//Get the list of end coords from the second formation
	vector<glm::vec3> coords = f2->getAgentCoords();

	//Set each agent's destination point to be this point, but relative to their current position
	for (int i=0; i<agents.size(); i++) {
		agents[i]->setEndPoint(coords[i]-f2->getCentre()+f1->getCentre());
	}

	//Set the crowd's pathVec to point towards the current destination.
	pathVec = normalize(currentDestination-currentPosition)*0.1f;
}

//Add more constructors - Formation, Sub-formation, Formation, Sub-formation, Path
//Formation, Sub-formation, Formation, Sub-formation, Path, Sub-path
//Formation, list of sub-formation, Formation, List of sub-formations, Path, list of sub-paths

Crowd::~Crowd(void)
{
}

void Crowd::update(vector<Agent*> neighbours)
{
	//v1 (done): Just update each agent individually, don't work out neighbours
	//v2 (done): All agents in the crowd are neighbours, none outside are
	//v3 (in-dev): Agents are also given the crowd vector, which follows the path. This is calculated each frame.
	//v4 (future): Neighbours are all agents in current crowd within a threshold
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
			//If so, pop the first one and set that as the destination.
			currentDestination = path.front();
			path.erase(path.begin());
			
			//Work out the vector to travel in, which is in that direction but a much smaller, static, speed.
			vec3 heading = normalize(currentDestination-currentPosition)*0.1f;
			for (int i=0; i<agents.size(); i++) {
				pathVec = heading;
			}
		} else {
			//Otherwise, the agents are at their destination, so set the path vector to 0.
			for (int i=0; i<agents.size(); i++) {
				pathVec = vec3(0, 0, 0);
			}
		}
		//If the point is not close enough, the current vector is alright so keep heading in this direction.
	}

	//Append agents to beginning of neighbours
	//Each iteration in this loop, replace i (or i-1?) with i+1 (or i?)
	glPushMatrix();
		glTranslated(currentPosition.x, 0, currentPosition.z);
		//glRotated(1, pathVec.x, 0, pathVec.z);
		glutSolidSphere(1.4, 20, 20);
	glPopMatrix();
	for (int i=0; i<agents.size(); i++) {
		//TODO - optimise this, give agents a pointer to pathVec when they are initialised, so there is no need to call it after this.
		agents[i]->update(neighbours);
		if (i<agents.size()-1) {
			neighbours[i] = agents[i+1];
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