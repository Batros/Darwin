#include "stdafx.h"
#include <iostream>
#include "Agent.h"
using namespace std;


Agent::Agent(glm::vec3 position) {
	this->position = position;
}

glm::vec3 Agent::getPosition()
{
	return position;
}

void Agent::setPosition(glm::vec3 newPosition)
{
	position = newPosition;
}

Agent::~Agent(void)
{
}


void Agent::update(vector<Agent*> neighbours, glm::vec3 endPoint)
{
	//Basic: Do nothing with neighbours, move 1/200th of the way to the end, with minimum and maximum speeds.
	setPosition(glm::vec3(position.x+(endPoint.x/200), position.y+(endPoint.y/200), position.z+(endPoint.z/200)));
	cout << min(4, 5);
}