#include "stdafx.h"
#include <iostream>
#include "Agent.h"
using namespace std;


Agent::Agent(glm::vec3 position) {
	this->position = position;
}

Agent::Agent(glm::vec3 position, glm::vec3 end) {
	this->position = position;
	this->endPoint = end;
}

glm::vec3 Agent::getPosition()
{
	return position;
}

void Agent::setEndPoint(glm::vec3 end) {
	endPoint = end;
}

void Agent::setPosition(glm::vec3 newPosition)
{
	position = newPosition;
}

Agent::~Agent(void)
{
}


void Agent::update(vector<glm::vec3> neighbours)
{
	//Basic: Do nothing with neighbours, move 1/200th of the way to the end, with minimum and maximum speeds.
	glm::vec3 newPos = position+glm::vec3((endPoint.x-position.x)/200, (endPoint.y-position.y)/200, (endPoint.z-position.z)/200);
	setPosition(newPos);
	glPushMatrix();
		glTranslated(position.x, position.y, position.z);
		glutSolidSphere(1.0f, 20, 20);
	glPopMatrix();
	//cout << min(4, 5);
}